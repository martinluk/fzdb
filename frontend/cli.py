#! /usr/bin/env python

import socket, select, string, sys, json, platform

# ======================================
# ========== Global Variables ==========
# ======================================

# Hard-coded values for now.
TCP_IP = 'localhost'
TCP_PORT = 1407

CHUNK_SIZE = 1024

COMMAND_UNKNOWN = -1
COMMAND_NONE = 0
COMMAND_QUIT = 1
COMMAND_HELP = 2
COMMAND_JSON_ON = 3
COMMAND_JSON_OFF = 4

sending = True
promptSymbolRequired = True
rawJson = False

# Keep a list of input files if they were provided.
inputFiles = sys.argv[1:]
noUserInput = True
if not inputFiles:
	noUserInput = False

# Create a socket to communicate on.
commSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
commSocket.settimeout(2)

# ===============================
# ========== Functions ==========
# ===============================

def printHelp():
	print("Commands:")
	print(":?              Show this help dialogue.")
	print(":json [on/off]  Switches raw JSON display on or off.")
	print(":quit           Quits the session.")
	
def classifyCommand(cmdstr):
	if cmdstr[0] != ':':
		return COMMAND_NONE

	# Switch statement simulation!
	return {
		"quit": COMMAND_QUIT,
		"?": COMMAND_HELP,
		"json on": COMMAND_JSON_ON,
		"json off": COMMAND_JSON_OFF
	}.get(cmdstr[1:], COMMAND_UNKNOWN)
	
def handleLocalCommand(command):
	global rawJson

	if command == COMMAND_QUIT:
		print("Quitting session.")
		commSocket.close()
		sys.exit()

	elif command == COMMAND_HELP:
		printHelp()

	elif command == COMMAND_JSON_ON:
		rawJson = True
		print("Raw JSON display turned on.")

	elif command == COMMAND_JSON_OFF:
		rawJson = False
		print("Raw JSON display turned off.")
		
	else:
		print("Unrecognised command.")
		
	promptSymbolRequired = True;
	
def readAllFromSocket(socket):
	readData = ""
	
	while True:
		# Try to read data.
		data = socket.recv(CHUNK_SIZE)
	
		# If we got 1024 bytes, this is 1023 data bytes + a padding null.
		# It indicates that we need to loop again to get the rest.
		if len(data) == CHUNK_SIZE:
			readData = readData + data[:-1]	# Trim the last null byte.
		else:
			readData = readData + data
			
			# This is the last chunk of data, so quit here.
			return readData

def sendOnSocket(socket, data):
	base = 0
	dataLength = len(data)
	
	while base < dataLength:
		sliceEnd = base + CHUNK_SIZE
		if sliceEnd > dataLength:
			sliceEnd = dataLength
			
		# For Python slicing begin:end, the slice goes up to end-1,
		# so to get 1023 bytes we pass a base offset of 1024.
		chunk = data[base:sliceEnd]
		
		# If we're sending a full chunk, add a final null byte.
		if len(chunk) == CHUNK_SIZE-1:
			chunk = chunk + "\0"
			
		# Send the data.
		socket.send(chunk)
		
		# Increment the base offset
		base = base + CHUNK_SIZE
		
def performReceiveData(socket):
	global sending
	global promptSymbolRequired

	# Read in the data.
	data = readAllFromSocket(socket)

	# If there wasn't actually any:
	if not data:
		# This signals that the connection has been closed.
		print("Connection to database closed.")
		commSocket.close()
		sys.exit()
	
	# Write the response to the console.
	if rawJson == True:
		sys.stdout.write(data.decode('utf-8'))
	else:
		jsonobj = json.loads(data.decode('utf-8'))
		if "response" in jsonobj:
			sys.stdout.write(jsonobj["response"])
		else:
			print("FATAL ERROR: No 'response' received via JSON.");
			print("Raw JSON received:\n")
			sys.stdout.write(data.decode('utf-8'))
			sys.stdout.write("\n\n");
			print("Closing connection.");
			commSocket.close()
			sys.exit()

	sys.stdout.write("\n");

	# Switch back into sending mode ready for the next command.
	sending = True

	# Display the prompt symbol next time!
	promptSymbolRequired = True
	
def performSendData(data):
	global sending

	# If the input was empty, just go round again.
	if not data:
		return
	
	# See if the input was a command.
	cmdType = classifyCommand(data)
	
	# If it was, handle it and continue to the next loop.
	if cmdType != COMMAND_NONE:
		handleLocalCommand(cmdType)
		return

	# Otherwise, send the input to the database.
	# Later we'll want to do JSON conversion here.
	else:
		sendOnSocket(commSocket, data.encode('utf-8'))

		# Switch into receiving mode to listen for a response back from the database.
		sending = False
	
def performSendStdinData(msg):
	performSendData(msg);

def performSendFileData():
	global inputFiles
	
	# Take a file name off the front of the list.
	filename = inputFiles[0]
	inputFiles = inputFiles[1:]
	
	# Read it.
	f = None
	fileData = ""
	try:
		f = open(filename, 'r')
		fileData = f.read()

	finally:
		if f is not None:
			f.close()
		else:
			print("ERROR: Unable to open file " + filename + ".")
			sys.exit()
	
	print("File data:")
	print(fileData)
	
	# Send the data.
	performSendData(fileData)
	
def waitForDataOn(socket, timeout):
	numFailedReads = 0
	
	sRead, sWrite, sError = select.select([socket], [], [], timeout)

	# Check to see whether we received any data. If this happens three times in a row,
	# assume the database is down and give up.
	if len(sRead) < 1:
		numFailedReads += 1
		if numFailedReads < 3:
			print("TIMEOUT: No response received from attempt %s of 3. Retrying..." % (numFailedReads))
		else:
			print("TIMEOUT: No response. Shutting down.")
			commSocket.close()
			sys.exit()
	
	return sRead[0]

def executeForUserInput():
	global promptSymbolRequired

	# The connection was successful.
	print("Connected to database on %s:%s. Type ':?' for help, or ':quit' to quit the session." % (TCP_IP, TCP_PORT))

	# Loop until we're told to stop:
	while True:
		if promptSymbolRequired:
			sys.stdout.write("$ ")
			sys.stdout.flush()
			promptSymbolRequired = False

		# If we're sending a command, we listen for data from the user.
		# No timeout is specified.
		if sending:
			performSendStdinData(sys.stdin.readline().strip())

		# Otherwise we listen from the database for a response.
		# This quits the script if the socket times out too many times.
		else:
			performReceiveData(waitForDataOn(commSocket, 30))

def executeForFileParsing():
	fileNo = 1
	while inputFiles:
		print("Executing input file " + str(fileNo) + ": " + inputFiles[0])
		performSendFileData()
		sys.stdout.write("\n");
		performReceiveData(waitForDataOn(commSocket, 30))
		sys.stdout.write("\n");
		fileNo = fileNo + 1
	
	print("All files processed, quitting.")
	sys.exit()
		
# ===============================
# ========== Main code ==========
# ===============================

# Try to connect.
try:
	commSocket.connect((TCP_IP, TCP_PORT))
except:	# We couldn't connect!
	print("ERROR: Unable to connect to database on %s:%s" % (TCP_IP, TCP_PORT))
	sys.exit()
	
# Execute the correct main loop.
if noUserInput == False:
	executeForUserInput()
else:
	executeForFileParsing()

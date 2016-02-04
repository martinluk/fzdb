#! /usr/bin/env python

import socket, select, string, sys, json

def printHelp():
    print("Commands:")
    print(":?              Show this help dialogue.")
    print(":json [on/off]  Switches raw JSON display on or off.")
    print(":quit           Quits the session.")

# Hard-coded values for now.
TCP_IP = 'localhost'
TCP_PORT = 1407

# Create a socket to communicate on.
commSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
commSocket.settimeout(2)

# Try to connect.
try:
	commSocket.connect((TCP_IP, TCP_PORT))
except:	# We couldn't connect!
	print("ERROR: Unable to connect to database on %s:%s" % (TCP_IP, TCP_PORT))
	sys.exit()

# The connection was successful.
print("Connected to database on %s:%s. Type ':?' for help, or ':quit' to quit the session." % (TCP_IP, TCP_PORT))

# We begin in sending mode, to listen for user input.
sending = True
promptSymbolRequired = True
rawJson = False
numFailedReads = 0;

# Loop until we're told to stop:
while True:
	if promptSymbolRequired:
		sys.stdout.write("$ ")
		sys.stdout.flush()
		promptSymbolRequired = False

        sRead = []
        sWrite = []
        sError = []

	# If we're sending a command, we listen for data from the user.
        # No timeout is specified.
	if sending:
                sRead, sWrite, sError = select.select(sys.stdin, [], [])

	# Otherwise we listen from the database for a response.
        # The timeout is 30 seconds - change this if required.
	else:
                sRead, sWrite, sError = select.select(commSocket, [], [], 30)

                # Check to see whether we received any data. If this happens three times in a row,
                # assume the database is down and give up.
                if len(sRead) < 1:
                        numFailedReads++
                        if numFailedReads < 3:
                                printf("TIMEOUT: No response received from attempt %s of 3. Retrying..." % (numFailedReads))
                        else:
                                printf("TIMEOUT: No response. Shutting down.")
                                commSocket.close()
                                sys.exit()


        # If there was no socket to read from:
	if len(sRead) < 1:
                # This shouldn't happen here!
                print("WARNING: No data received on socket! This is probably a bug.")
                continue
	
	# A socket has data!
        numFailedReads = 0;
	sock = sRead[0]

	# If the socket is stdin, parse the command.
	if sock == sys.stdin:
                msg = sys.stdin.readline().rstrip("\n").strip()

		# If the input was empty, just go round again.
		if not msg:
			continue

		# If the user wants to quit, do so.
		if msg == ":quit":
			print("Quitting session.")
			commSocket.close()
			sys.exit()

                elif msg == ":?":
                        printHelp()
                        promptSymbolRequired = True
                        continue

                # Dumb but quick!
                elif msg == ":json on":
                        rawJson = True
                        print("Raw JSON display turned on.")
                        promptSymbolRequired = True
                        continue

                elif msg == ":json off":
                        rawJson = False
                        print("Raw JSON display turned off.")
                        promptSymbolRequired = True;
                        continue

		# Otherwise, send the command.
		# Later we'll want to do JSON conversion here.
		else:
			commSocket.send(msg.encode('utf-8'))

			# Switch into receiving mode to listen for a response back from the database.
			sending = False
	
	# If the socket is our communications socket, parse the database response.
	elif sock == commSocket:
		# Read in the data.
		data = commSocket.recv(4096)

		# If there wasn't actually any:
		if not data:
			# This signals that the connection has been closed.
			print("Connection to database closed.")
			commSocket.close()
			sys.exit()
		else:
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

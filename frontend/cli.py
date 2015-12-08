#! /usr/bin/env python

import socket, select, string, sys

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
	print "ERROR: Unable to connect to database on %s:%s" % (TCP_IP, TCP_PORT)
	sys.exit()

# The connection was successful.
print "Connected to database on %s:%s. Type ':quit' to quit the session." % (TCP_IP, TCP_PORT)

# We begin in sending mode, to listen for user input.
sending = True
promptSymbolRequired = True

# Loop until we're told to stop:
while True:
	if promptSymbolRequired:
		sys.stdout.write("$ ")
		sys.stdout.flush()
		promptSymbolRequired = False

	socketToRead = []

	# If we're sending a command, we listen for data from the user.
	if sending:
		socketToRead = [sys.stdin]
	# Otherwise we listen from the database for a response.
	else:
		socketToRead = [commSocket]
	
	# Get any information from our chosen socket.
	sRead, sWrite, sError = select.select(socketToRead, [], [])

	# If there was no socket to read from, just go round again I guess.
	if len(sRead) < 1:
		continue
	
	# A socket has data!
	sock = sRead[0]

	# If the socket is stdin, parse the command.
	if sock == sys.stdin:
		msg = sys.stdin.readline().rstrip("\n")

		# If the input was empty, just go round again.
		if not msg:
			continue

		# If the user wants to quit, do so.
		if msg == ":quit":
			print "Quitting session."
			commSocket.close()
			sys.exit()

		# Otherwise, send the command.
		# Later we'll want to do JSON conversion here.
		else:
			commSocket.send(msg)

			# Switch into receiving mode to listen for a response back from the database.
			sending = False
	
	# If the socket is our communications socket, parse the database response.
	elif sock == commSocket:
		# Read in the data.
		data = commSocket.recv(4096)

		# If there wasn't actually any:
		if not data:
			# This signals that the connection has been closed.
			print "Connection to database closed."
			commSocket.close()
			sys.exit()
		else:
			# Write the response to the console.
			sys.stdout.write(data)

			# Switch back into sending mode ready for the next command.
			sending = True

			# Display the prompt symbol next time!
			promptSymbolRequired = True

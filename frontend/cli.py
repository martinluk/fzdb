#! /usr/bin/env python

import socket, select, string, sys

TCP_IP = 'localhost'
TCP_PORT = 1407 

def readIncoming(socket):
	data = sock.recv(4096)
	if not data:
		return False
	else:
		sys.stdout.write("> ")
		sys.stdout.write(data)
		return True

def getChar(Block=False):
	if Block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
		return sys.stdin.read(1)
	raise error('NoChar')

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(2)

# Try to connect.
try:
	s.connect((TCP_IP, TCP_PORT))
except:
	print "ERROR: Unable to connect to database on %s:%s" % (TCP_IP, TCP_PORT)
	sys.exit()

print "Connected to database on %s:%s. Type ':quit' to quit the session." % (TCP_IP, TCP_PORT)

while True:
	socket_list = [sys.stdin, s]
	read_sockets, write_sockets, error_sockets = select.select(socket_list, [], [])

	for sock in read_sockets:
		if sock == s:
			# Incoming data!
			if not readIncoming(s):
				print "Connection to database closed."
				s.close()
				sys.exit()

		elif sock == sys.stdin:
			# User entered a command.
			sys.stdout.write("$ ")
			msg = sys.stdin.readline().rstrip("\n")
			if not msg:
				continue
			if msg == ":quit":
				print "Quitting session."
				s.close()
				sys.exit()
			else:
				s.send(msg)
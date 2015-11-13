#! /usr/bin/env python

# Thanks to http://stackoverflow.com/questions/4342697/tcp-port-using-python-how-to-forward-command-output-to-tcp-port
import socket
import sys

TCP_IP = 'localhost'
TCP_PORT = 1407 
CMD_QUIT = "quit"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
print "Connected - type 'quit' to disconnect."
while True:
	line = sys.stdin.readline().rstrip();
	if line != "quit":
		s.send(line)
	else:
		break

s.close()


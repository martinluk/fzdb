#! /usr/bin/env python

# VERY rudimentary script loader

import socket, select, string, sys, json, time
import argparse;

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('file', help='A FUZ file with the data to load')

args = parser.parse_args()

# Hard-coded values for now.
TCP_IP = 'localhost'
TCP_PORT = 1407

with open(args.file, 'r') as myfile:

	data=myfile.read()

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

	commSocket.send(data.encode('utf-8'))
	time.sleep(1)
	print("done")

	commSocket.close()
	sys.exit()
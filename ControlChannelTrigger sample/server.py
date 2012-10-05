import socket
import sys
import os
import threading

class ServerThread ( threading.Thread ):
	def __init__( self, clientsock, socketnum ):
			threading.Thread.__init__(self)
			self.clientsocket = clientsock
			self.socketnum = socketnum
	def run ( self ):
		self.clientsocket.settimeout(5)
		messagenum = 1
		datastring = '0123456789'
		#for n in range(1, 50000):
		#	datastring = datastring + '0123456789'
		while 1:
			readbuf = ""
			try:
				readbuf = self.clientsocket.recv(48)
			except socket.error, msg:
				pass
			if len(readbuf):
				print 'received: '
				print readbuf
				print '\n'
			send_string = str(messagenum) + '_' + datastring
			messagenum = messagenum + 1
#			self.clientsocket.sendall(send_string)
#			print self.socketnum, ': message ', messagenum, 'sent ', len(send_string), ' bytes\n'


#create an INET, STREAMing socket
serversocket = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)
#bind the socket to a public host,
# and a well-known port
print 'binding... \n'
serversocket.bind(('', 5600))
#become a server socket
serversocket.listen(1)
numsockets = 1

while 1:
	#accept connections from outside
	(clientsocket, address) = serversocket.accept()
	#now do something with the clientsocket
	print 'accepted connection\n'
	ServerThread(clientsocket, numsockets).start()
	numsockets = numsockets + 1
	print 'waiting for connection ', numsockets

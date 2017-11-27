import socket
import sys
from threading import Thread

break_word = "exit"

def wait_message(s):
	in_data = s.recv(2048) 
	in_data = str(in_data, errors='ignore')
	print(in_data)
	return 
	
def send_message(s):
	out_data = input(">")
	if(out_data == break_word):
		global state
		state = False
		sys.exit(1)#тут завершается онли 1 поток
	s.send(out_data.encode()) 
	
	return 





def inf_send(s):
	while state:
		send_message(s)
		
def inf_wait(s):
	while state:
		wait_message(s)


state = True
		
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = 'localhost'
port = 8021
print(host)
s.connect((host, port))
print("#connected")


t_send = Thread(target=inf_send, args=(s,))
t_wait = Thread(target=inf_wait, args=(s,))

t_send.start()
t_wait.start()


while state:
    pass
	#mychat.send_message(s)
	#mychat.wait_message(s)


	#out_data = input('>')
	#s.send(out_data.encode())  
	#in_data = s.recv(1000000) 
	#print(in_data.decode())
	
print("end")
s.close()

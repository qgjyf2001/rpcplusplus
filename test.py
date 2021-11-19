import socket
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
dest_addr = ("127.0.0.1", 8080)
tcp_socket.connect(dest_addr)
tcp_socket.send(b'00075\r\n{"name":"add","param1":[12345,23456,34567],"param2":["aaab","bbbb","cccc"]}\r\n')
recv_data = tcp_socket.recv(1024)
print(recv_data)
tcp_socket.close()

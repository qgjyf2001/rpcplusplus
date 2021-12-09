from python.serialization import *
from python.rpcParser import *
import socket
class rpcClient():
    pass
class rpcCall():
    def __init__(self,client:rpcClient,params:int,name:str):
        self.params=params
        self.name=name
        self.client=client
    def __call__(self, *args, **kwds):
        assert(len(args)==self.params)
        data={}
        data["name"]=self.name
        for count,param in enumerate(args[::-1]):
            data["param"+str(count+1)]=param
        return unSerialization(self.client.remoteCall(serialization(data)))
class rpcClient():
    def __init__(self,ipAddress,port):
        self.tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp_socket.connect((ipAddress,port))
    def remoteCall(self,data:str):
        length=str(len(data))
        length='0'*(5-len(length))+length
        bData=(length+"\r\n"+data+"\r\n").encode()
        self.tcp_socket.send(bData)
        recv=self.tcp_socket.recv(1024).decode()
        mp=parse(recv)
        length,message=mp["length"],mp["message"]
        while (length>len(message)):
            message+=self.tcp_socket.recv(1024).decode()
        return message
    def __del__(self):
        self.tcp_socket.close()
    def makeRpcCall(self,f):
        argnum=f.__code__.co_argcount
        return rpcCall(self,argnum,f.__name__)

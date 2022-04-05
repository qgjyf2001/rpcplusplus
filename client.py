from python.rpcClient import rpcClient

def getService(a:str):
    return None
def newService(a:str,b):
    return None
client=rpcClient("127.0.0.1",8080)
newService=client.makeRpcCall(newService)
getService=client.makeRpcCall(getService)
newService("test.service",['127.0.0.1:10011'])
result=getService('test.service')
print(result)
print(type(result))

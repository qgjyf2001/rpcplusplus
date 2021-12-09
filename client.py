from python.rpcClient import rpcClient
def add(a:list,b:list)->list:
    return None
client=rpcClient("127.0.0.1",8080)
add=client.makeRpcCall(add)
result=add(['abcde','fghij','klimn','opqrs'],[12345,23456,34567,45789])
print(result)
print(type(result))
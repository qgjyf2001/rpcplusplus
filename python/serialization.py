import json
def serialization(data:object)->str:
    return json.dumps(data)
def unSerialization(data:str):
    return json.loads(data)
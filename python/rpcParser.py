def parse(data:str):
    length=0
    try:
        length=int(data[:5])
        assert(data[5:7]=="\r\n")
        length2=len(data)
        assert(length2-9<=length)
        if (length2-9==length):
            return {"length":length,"message":data[7:-2]}
        else:
            return {"length":length,"message":data[7:]}
    except:
        raise Exception("invalid request")
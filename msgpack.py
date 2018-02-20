"""
.. module:: msgpack

*******
MsgPack
*******

This module define functions to serialize and unserialize objects to and from `msgpack <http://msgpack.org>`_ format.

Objects serialized with msgpack are usually smaller than their equivalent json representation.


    """


new_exception(MsgPackError,Exception)
new_exception(MsgUnpackError,Exception)

def _pack(obj,res):
    t = type(obj)
    if t==PDICT:
        _pack_map(obj,res)
    elif t == PLIST or t==PTUPLE:
        _pack_array(obj,res)
    elif t == PSMALLINT:
        if obj>=0:
            if obj<=0x7f:
                res.append(obj)
            elif obj<=0xff:
                res.append(0xcc)
                res.append(0)
                __pack("B",res,0,-1,obj)
            elif obj<=0xffff:
                res.append(0xcd)
                res.extend("\x00\x00")
                __pack(">H",res,0,-2,obj)
            else:
                res.append(0xce)
                res.extend("\x00\x00\x00\x00");        
                __pack(">I",res,0,-4,obj)
        else:
            if obj>=-128:
                res.append(0xd0)
                res.append(0)
                __pack("b",res,0,-1,obj)
            elif obj>=-32768:
                res.append(0xd1)
                res.extend("\x00\x00")
                __pack(">h",res,0,-2,obj)
            else:
                res.append(0xd2)
                res.extend("\x00\x00\x00\x00");        
                __pack(">I",res,0,-4,obj)        
    elif t == PSTRING or t == PBYTEARRAY or t == PBYTES: 
        lb = ___len(obj)
        if t==PSTRING:
            if lb<=31:                
                res.append(0b10100000+lb)
            elif lb<=255:
                res.append(0xd9)
                res.append(lb)
            elif lb<=65535:
                res.append(0xda)
                res.extend("\x00\x00");
                __pack(">H",res,0,-2,lb)
        else:
            if lb<=255:
                res.append(0xc4)
                res.append(lb)
            elif lb<=65535:
                res.append(0xc5)
                res.extend("\x00\x00");
                __pack(">H",res,0,-2,lb)
        res.extend(obj)
    elif t==PFLOAT:
        res.append(0xcb)
        res.extend("\x00\x00\x00\x00");
        res.extend("\x00\x00\x00\x00");
        __pack("d",res,0,-8,obj)
    elif t == PBOOL:
        if obj:
            res.append(0xc3)
        else:
            res.append(0xc2)
    elif obj == None:
        res.append(0xc0)
    else:
        raise MsgPackError

def _pack_array(obj,res):
    lb = ___len(obj)
    if lb<=15:
        res.append(0b10010000+lb)
    else:
        res.append(0xdc)
        res.extend("\x00\x00");
        __pack(">H",res,0,-2,lb)
    for o in obj:
        _pack(o,res)

def _pack_map(obj,res):
    lb = ___len(obj)
    if lb<=15:
        res.append(0b10000000+lb)
    else:
        res.append(0xde)
        res.extend("\x00\x00");
        __pack(">H",res,0,-2,lb)
    for k,v in obj.items():
        _pack(k,res)
        _pack(v,res)

def pack(obj):
    """
.. function:: pack(obj)

    Returns a bytearray containing the msgpack representation of *obj*.

    Raises ``MsgPackError`` when *obj* contains non serializable objects.    

    """    
    res = bytearray()
    _pack(obj,res)
    return res


def unpack(data,offs=0):
    """
.. function:: unpack(data,offs=0)

    Returns an object represented in msgpack format inside the byte sequence *data* starting from offset *offs*.

    Not every valid msgpack representation can be converted to python objects by *unpack*. 
    For example, 64-bit msgpack integers and msgpack ext types. In that case, ``MsgUnpackError`` is raised.
    """    
    if len(data)-offs>0:
        obj = []
        _unpack(data,offs,obj)
        return obj[0]
    return None

def _unpack(data,pos,obj):
    c = __byte_get(data,pos)
    pos+=1
    if c<=0x7f:
        #fixnum
        dp = c        
    elif c>=0b11100000:
        #negative fixnum: todo
        dp = -1
        raise MsgUnpackError        
    elif c == 0xc0:
        dp = None
    elif c == 0xc2:
        dp = False    
    elif c == 0xc3:
        dp = True
    elif c >= 0xcc and c<=0xce:
        #unsigned int
        if c==0xcc:
            dp = __byte_get(data,pos)
            pos+=1
        elif c==0xcd:    
            dp = __unpack(">H",data,0,pos)
            pos+=2            
        else:
            dp = __unpack(">I",data,0,pos)
            pos+=4     
    elif c>=0xd0 and c<=0xd2:
        #signed int
        if c==0xd0:
            dp = __unpack(">b",data,0,pos)
            pos+=1
        elif c==0xd1:                
            dp = __unpack(">h",data,0,pos)
            pos+=2
        else:
            dp = __unpack(">i",data,0,pos)
            pos+=4
    elif c>=0b10100000 and c<=0b10111111:
        dp =c-0b10100000
        pos+=dp
        dp = str(data[pos-dp:pos])
    elif c==0xd9:
        dp = data[pos]
        pos+=1+dp
        dp = str(data[pos-dp:pos])
    elif c == 0xda:
        dp = __unpack(">H",data,0,pos)
        pos+=2+dp
        dp = str(data[pos-dp:pos])
    elif c == 0xdb:
        dp = __unpack(">I",data,0,pos)
        pos+=4+dp
        dp = str(data[pos-dp:pos])
    elif c==0xc4:
        dp = __byte_get(data,pos)
        pos+=1+dp
        dp = bytes(data[pos-dp:pos])
    elif c==0xc5:
        dp = __unpack(">H",data,0,pos)
        pos+=2+dp
        dp=bytes(data[pos-dp:pos])
    elif c==0xc6:
        dp = __unpack(">I",data,0,pos)
        pos+=4+dp
        dp=bytes(data[pos-dp:pos])
    elif c>=0b10010000 and c<=0b10011111:
        dp = []
        pos = _unpack_array(data,pos,c-0b10010000,dp)
    elif c==0xdc:
        dp = []        
        pos =_unpack_array(data,pos+2,__unpack(">H",data,0,pos),dp)
    elif c==0xdd:
        dp = []        
        pos =_unpack_array(data,pos+4,__unpack(">I",data,0,pos),dp)
    elif c>=0b10000000 and c<=0b10001111:
        dp = {}
        pos = _unpack_map(data,pos,c-0b10000000,dp)
    elif c==0xde:
        dp = {}
        pos = _unpack_array(data,pos+2,__unpack(">H",data,0,pos),dp)
    elif c==0xdf:
        dp = {}
        pos = _unpack_array(data,pos+4,__unpack(">I",data,0,pos),dp)
    else: 
        #64 bits things: c == 0xcf or c== 0xd3:
        #ext types
        raise MsgUnpackError
    obj.append(dp)
    return pos


def _unpack_array(data,pos,n,obj):
    for i in range(0,n):
        pos = _unpack(data,pos,obj)
    return pos

def _unpack_map(data,pos,n,obj):
    key = []
    val = []
    for i in range(0,n):
        pos = _unpack(data,pos,key)
        pos = _unpack(data,pos,val)
        obj[key[0]]=val[0]
        key.clear()
        val.clear()
    return pos
"""
.. module:: msgpack

*******
MsgPack
*******

This module define functions to serialize and unserialize objects to and from `msgpack <http://msgpack.org>`_ format.

Objects serialized with msgpack are usually smaller than their equivalent json representation.

The supported formats are shown in the table below.

+-----------------+----------------------------+-------------------------+
| **Format name** | **First byte (in binary)** | **First byte (in hex)** |
+-----------------+----------------------------+-------------------------+
| positive fixint | 0xxxxxxx                   | 0x00 - 0x7f             |
+-----------------+----------------------------+-------------------------+
| fixmap          | 1000xxxx                   | 0x80 - 0x8f             |
+-----------------+----------------------------+-------------------------+
| fixarray        | 1001xxxx                   | 0x90 - 0x9f             |
+-----------------+----------------------------+-------------------------+
| fixstr          | 101xxxxx                   | 0xa0 - 0xbf             |
+-----------------+----------------------------+-------------------------+
| nil             | 11000000                   | 0xc0                    |
+-----------------+----------------------------+-------------------------+
| false           | 11000010                   | 0xc2                    |
+-----------------+----------------------------+-------------------------+
| true            | 11000011                   | 0xc3                    |
+-----------------+----------------------------+-------------------------+
| bin 8           | 11000100                   | 0xc4                    |
+-----------------+----------------------------+-------------------------+
| bin 16          | 11000101                   | 0xc5                    |
+-----------------+----------------------------+-------------------------+
| float 32        | 11001010                   | 0xca                    |
+-----------------+----------------------------+-------------------------+
| uint 8          | 11001100                   | 0xcc                    |
+-----------------+----------------------------+-------------------------+
| uint 16         | 11001101                   | 0xcd                    |
+-----------------+----------------------------+-------------------------+
| uint 32         | 11001110                   | 0xce                    |
+-----------------+----------------------------+-------------------------+
| int 8           | 11010000                   | 0xd0                    |
+-----------------+----------------------------+-------------------------+
| int 16          | 11010001                   | 0xd1                    |
+-----------------+----------------------------+-------------------------+
| int 32          | 11010010                   | 0xd2                    |
+-----------------+----------------------------+-------------------------+
| str 8           | 11011001                   | 0xd9                    |
+-----------------+----------------------------+-------------------------+
| str 16          | 11011010                   | 0xda                    |
+-----------------+----------------------------+-------------------------+
| array 16        | 11011100                   | 0xdc                    |
+-----------------+----------------------------+-------------------------+
| map 16          | 11011110                   | 0xde                    |
+-----------------+----------------------------+-------------------------+
| negative fixint | 111xxxxx                   | 0xe0 - 0xff             |
+-----------------+----------------------------+-------------------------+

    """

import struct

new_exception(ZMsgPackError, Exception)
new_exception(MsgUnpackError, Exception)

def _pack(obj, res):
    t = type(obj)
    if t == PDICT:
        _pack_map(obj, res)
    elif t == PSHORTS or t == PSHORTARRAY or t == PLIST or t == PTUPLE:
        _pack_array(obj, res)
    elif t == PSMALLINT or t == PINTEGER:
        if obj >= 0:
            if obj <= 0x7f:
                res.append(obj)
            elif obj <= 0xff:
                res.append(0xcc)
                res.append(obj)
            elif obj <= 0xffff:
                res.append(0xcd)
                res.extend(struct.pack('>H', obj))
            else:
                res.append(0xce)
                res.extend(struct.pack('>I', obj))
        else:
            if obj >= -32:
                res.extend(struct.pack('b', obj))
            elif obj >= -128:
                res.append(0xd0)
                res.extend(struct.pack('b', obj))
            elif obj >= -32768:
                res.append(0xd1)
                res.extend(struct.pack('>h', obj))
            else:
                res.append(0xd2)
                res.extend(struct.pack('>i', obj))
    elif t == PSTRING or t == PBYTEARRAY or t == PBYTES: 
        lb = ___len(obj)
        if t == PSTRING:
            if lb <= 31:
                res.append(0b10100000+lb)
            elif lb <= 255:
                res.append(0xd9)
                res.append(lb)
            elif lb <= 65535:
                res.append(0xda)
                res.extend(struct.pack('>H', lb))
        else:
            if lb <= 255:
                res.append(0xc4)
                res.append(lb)
            elif lb <= 65535:
                res.append(0xc5)
                res.extend(struct.pack('>H', lb))
        res.extend(obj)
    elif t == PFLOAT:
        res.append(0xca)
        res.extend(struct.pack('>f', obj))
    elif t == PBOOL:
        if obj:
            res.append(0xc3)
        else:
            res.append(0xc2)
    elif obj == None:
        res.append(0xc0)
    else:
        raise ZMsgPackError

def _pack_array(obj,res):
    lb = ___len(obj)
    if lb <= 15:
        res.append(0b10010000+lb)
    else:
        res.append(0xdc)
        res.extend(struct.pack('>H', lb))
    for o in obj:
        _pack(o,res)

def _pack_map(obj,res):
    lb = ___len(obj)
    if lb <= 15:
        res.append(0b10000000+lb)
    else:
        res.append(0xde)
        res.extend(struct.pack('>H',lb))
    for k, v in obj.items():
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


def unpack(data, offs=0):
    """
.. function:: unpack(data,offs=0)

    Returns an object represented in msgpack format inside the byte sequence *data* starting from offset *offs*.

    Not every valid msgpack representation can be converted to python objects by *unpack*. 
    For example, 64-bit msgpack integers and msgpack ext types. In that case, ``MsgUnpackError`` is raised.
    """
    if len(data)-offs>0:
        obj = []
        _unpack(data, offs, obj)
        return obj[0]
    return None

def _unpack(data, pos, obj):
    c = __byte_get(data, pos)
    
    pos+=1
    if c <= 0x7f:
        dp = c
    elif c>=0b11100000:
        dp = struct.unpack('b',c)[0]
    elif c == 0xc0:
        dp = None
    elif c == 0xc2:
        dp = False
    elif c == 0xc3:
        dp = True
    
    elif c >= 0xcc and c <= 0xce:
        if c == 0xcc:
            dp = __byte_get(data,pos)
            pos += 1
        elif c == 0xcd:
            dp = struct.unpack('>H', data[pos:pos+2])[0]
            pos += 2
        else:
            dp = struct.unpack('>I', data[pos: pos+4])[0]
            pos += 4
    elif c >= 0xd0 and c <= 0xd2:
        if c == 0xd0:
            dp = struct.unpack("b",data[pos])[0]
            pos += 1
        elif c == 0xd1:
            dp = struct.unpack(">h", data[pos : pos+2])[0]
            pos += 2
        else:
            dp = struct.unpack(">i", data[pos : pos+4])[0]
            pos += 4
    elif c == 0xca:
        dp = struct.unpack(">f", data[pos : pos+4])[0]
        pos += 4
    elif c >= 0b10100000 and c <= 0b10111111:
        dp = c-0b10100000
        pos += dp
        dp = str(data[pos-dp:pos])
    elif c == 0xd9:
        dp = data[pos]
        pos += 1+dp
        dp = str(data[pos-dp:pos])
    elif c == 0xda:
        dp = struct.unpack(">H",data[pos:pos+2])[0]
        pos += 2+dp
        dp = str(data[pos-dp:pos])
    elif c == 0xc4:
        dp = __byte_get(data,pos)
        pos += 1+dp
        dp = bytes(data[pos-dp:pos])
    elif c==0xc5:
        dp = struct.unpack(">H",data[pos:pos+2])[0]
        pos+=2+dp
        dp=bytes(data[pos-dp:pos])
    elif c>=0b10010000 and c<=0b10011111:
        dp = []
        pos = _unpack_array(data,pos,c-0b10010000,dp)
    elif c==0xdc:
        dp = []
        pos =_unpack_array(data, pos+2, struct.unpack(">H",data[pos:pos+2])[0], dp)
    elif c >= 0b10000000 and c <= 0b10001111:
        dp = {}
        pos = _unpack_map(data, pos, c-0b10000000, dp)
    elif c == 0xde:
        dp = {}
        pos = _unpack_map(data, pos+2, struct.unpack(">H",data[pos:pos+2])[0], dp)
    else: 
        raise MsgUnpackError
    obj.append(dp)
    return pos


def _unpack_array(data, pos, n, obj):
    for i in range(0, n):
        pos = _unpack(data, pos, obj)
    return pos

def _unpack_map(data, pos, n, obj):
    key = []
    val = []
    for i in range(0, n):
        pos = _unpack(data, pos, key)
        pos = _unpack(data, pos, val)
        obj[key[0]] = val[0]
        key.clear()
        val.clear()
    return pos



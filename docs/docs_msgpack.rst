.. module:: msgpack

*******
MsgPack
*******

This module define functions to serialize and unserialize objects to and from `msgpack <http://msgpack.org>`_ format.

Objects serialized with msgpack are usually smaller than their equivalent json representation.


    
.. function:: pack(obj)

    Returns a bytearray containing the msgpack representation of *obj*.

    Raises ``MsgPackError`` when *obj* contains non serializable objects.    

    
.. function:: unpack(data,offs=0)

    Returns an object represented in msgpack format inside the byte sequence *data* starting from offset *offs*.

    Not every valid msgpack representation can be converted to python objects by *unpack*. 
    For example, 64-bit msgpack integers and msgpack ext types. In that case, ``MsgUnpackError`` is raised.
    

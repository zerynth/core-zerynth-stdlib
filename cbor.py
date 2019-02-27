"""
.. module:: cbor

****
CBOR
****

This module define functions to serialize and deserialize objects to and from `CBOR <http://cbor.io>`_ format.
The serialization and deserialization of objects is performed using a wrapped version of the awesome and lighting fast `libcbor  <http://libcbor.org/>`_

    """


@native_c("_cbor_init",[
    "csrc/cbor/cbor_ifc.c",
    "csrc/cbor/libcbor-0.5.0/src/*",
    "csrc/cbor/libcbor-0.5.0/src/cbor/*",
    "csrc/cbor/libcbor-0.5.0/src/cbor/internal/*"
    ],
    [],
    [
        "-I.../csrc/cbor/libcbor-0.5.0/src",
        "-I.../csrc/cbor/libcbor-0.5.0/src/cbor",
        "-I.../csrc/cbor/libcbor-0.5.0/src/cbor/internal"
        ])
def init(tagclass,undefclass,tagname,valuename):
    pass




@native_c("_cbor_loads",[])
def _cbor_loads(buf):
    pass

@native_c("_cbor_dumps",[])
def _cbor_dumps(obj):
    pass


def loads(buf):
    """
.. function:: loads(data)

    Returns a Python object represented by the byte sequence *data*.
    For CBOR specific structures such as *tags* and *undefined* values, 
    the function returns instances of the :class:`Tag` and :class:`Undefined` classes.

    Raises ``ValueError`` when *data* contains bad or unsupported CBOR.

    """
    return _cbor_loads(buf)


def dumps(obj):
    """
.. function:: dumps(obj)

    Returns a bytes object containing the CBOR representation of *obj*.
    If a Python object is not serializable to CBOR, it is serialized to :class:`Undefined`.
    The function always produces definite major types (bytestrings, string, arrays and maps).
    To generate CBOR specific structures such as *tags* and *undefined* values, pass as argument instances
    of the :class:`Tag` and :class:`Undefined` classes.

    Raises ``RuntimeError`` when *obj* can't be serialized.

    """ 
    return _cbor_dumps(obj)


class Tag():
    """
=========
Tag class
=========

.. class:: Tag(tag, value)

    Create a Tag instance. Tag instances have two attributes, :samp:`tag` and :samp:`value` that can be manually changed if needed.
    :samp:`tag` must be an integer while :samp:`value` can be any CBOR serializable python object.
    An instance of this class is returned during deserialization if a tag is found.

    """
    def __init__(self,thetag,thevalue):
        self.tag=thetag
        self.value=thevalue
    def _get_names(self):
        return __nameof(tag),__nameof(value)
    def __str__(self):
        return "TAG("+str(self.tag)+":"+str(self.value)+")"

class Undefined():
    """
===============
Undefined class
===============

.. class:: Undefined()

    Create an Undefined instance. Since Python has no undefined values, this class is used to both serialize and deserialize this kind of data.

    """
    def __str__(self):
        return "undefined"

def init_lib():
    atag = Tag(0,0)
    tagname, valuename = atag._get_names()
    init(Tag,Undefined,tagname,valuename)

init_lib()


"""
.. module:: json

****
JSON
****

This module define functions to serialize and deserialize objects to and from `JSON <http://json.org>`_ format.
The deserialization of objects is performed using a wrapped version of the awesome and lighting fast `JSMN library  <http://zserge.com/jsmn.html>`_

    """


new_exception(JSONError,Exception)

_to_escape = "\"\\\b\f\n\r\t"

class _DumpCounter():
    def __init__(self):
        self.l = 0
    def append(self,x):
        self.l+=1
    def extend(self,obj):
        self.l+=len(obj)
    def __setitem__(self, key, value):
        pass
    def __str__(self):
        return str(self.l)

class _DumpBuilder():
    def __init__(self,n):
        self.data = bytes(n)
        self.pos = 0
    def append(self,x):
        __byte_set(self.data,self.pos,x)
        self.pos+=1
    def extend(self,obj):
        for i in range(___len(obj)):
            __byte_set(self.data,self.pos,__byte_get(obj,i))
            self.pos+=1
    def __setitem__(self, key, value):
        if key==-1:
            __byte_set(self.data,self.pos-1,value)

def _dumps(obj,res):
    t = type(obj)
    if t==PDICT:
        _dumps_map(obj,res)
    elif t == PLIST or t==PTUPLE:
        _dumps_array(obj,res)
    elif t == PSMALLINT or t==PFLOAT or t==PINTEGER:
        res.extend(str(obj))
    elif t == PSTRING or t == PBYTEARRAY or t == PBYTES: 
        lb = ___len(obj)
        res.append(__ORD("\""))
        for i in range(0,lb):
            c = __byte_get(obj,i)
            if c in _to_escape:
                res.append(__ORD("\\"))
                if c==__ORD('\n'):
                    res.append(__ORD("n"))
                elif c==__ORD('\r'):
                    res.append(__ORD("r"))
                elif c==__ORD('\t'):
                    res.append(__ORD("t"))
                elif c==__ORD('\f'):
                    res.append(__ORD("f"))
                elif c==__ORD('\b'):
                    res.append(__ORD("b"))
                else:
                    res.append(c)                        
            elif c<0x20 or c>0x7e:
                res.extend("\\u00")
                res.extend(hex(c,""))            
            else:
                res.append(c)
        res.append(__ORD("\""))
    elif t == PBOOL:
        if obj:
            res.extend("true")
        else:
            res.extend("false")
    elif obj == None:
        res.extend("null")
    else:
        raise JSONError

def _dumps_array(obj,res):
    lb = ___len(obj)
    res.append(__ORD("["))
    if lb:
        _dumps(obj[0],res)
        for i in range(1,lb):
            res.append(__ORD(","))
            _dumps(obj[i],res)
    res.append(__ORD("]"))

def _dumps_map(obj,res):
    lb = ___len(obj)
    res.append(__ORD("{"))
    for k,v in obj.items():
        if type(k)!=PSTRING:
            raise JSONError
        _dumps(k,res)
        res.append(__ORD(":"))
        _dumps(v,res)
        res.append(__ORD(","))
    if lb:
        res[-1]=__ORD("}")
        #__byte_set(res,___len(res)-1,__ORD("}"))
    else:
        res.append(__ORD("}"))

def dumps(obj):
    """
.. function:: dumps(obj)

    Returns a bytearray containing the JSON representation of *obj*.

    Raises ``JSONError`` when *obj* contains non serializable objects.

    """    
    res = _DumpCounter()# bytearray()    
    _dumps(obj,res)
    res = _DumpBuilder(res.l)
    _dumps(obj,res)
    __change_type(res.data,PSTRING)
    return res.data


@native_c("jsmn_loads",["csrc/jsmn/*"])
def _loads(data):
    pass

def loads(data):
    """
.. function:: loads(data)

    Returns the object represented in JSON format inside the byte sequence *data*.

    Raises ``JSONError`` when *data* contains bad JSON.

    """    
    if len(data)>0:
        try:
            obj = _loads(data)
            return obj
        except:
            raise JSONError
    else:
        return None

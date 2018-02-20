"""
.. module: sha3


****************
SHA3 secure hash
****************

This module implements the interface to NIST’s secure hash algorithm, known as SHA-3. 
It is used in the same way as all crypto hash modules: an instance of the SHA3 class is
created, then feed this object with arbitrary strings/bytes using the update() method, and at any point you can ask it for the digest of the
concatenation of the strings fed to it so far.

The class supports 4 variants of SHA3, selectable in the constructor with one of the following constants:

    * :samp:`SHA224`
    * :samp:`SHA256`
    * :samp:`SHA284`
    * :samp:`SHA512`

The module is based on the C library `cifra <https://github.com/ctz/cifra>`_.

"""

@c_native("zs_sha3_init",["csrc/sha3_ifc.c","csrc/cifra/src/sha3.c","csrc/cifra/src/blockwise.c"],[],["-I.../csrc/cifra/src","-I.../csrc/cifra/src/ext"])
def __hash_init(hashtype):
    pass

@c_native("zs_sha3_update",[])
def __hash_update(hashtype,ctx,data):
    pass

@c_native("zs_sha3_digest",[])
def __hash_digest(hashtype,ctx):
    pass

@c_native("zs_sha3_context",[])
def __make_context(hashtype,ctx):
    pass



SHA224 = 0
SHA256 = 1
SHA384 = 2
SHA512 = 3


class SHA3():
    """
==============
The SHA3 class
==============
    
.. class:: SHA3(hashtype=SHA256)

       This class allows the generation of SHA3 hashes. It is thread safe. By default, it calculates the SHA256 variant
       of SHA3. This behaviour can be changed by passing a different value for *hashtype*
    """
    def __init__(self,hashtype=SHA256):
        self.hashtype = hashtype
        self.ctx = __hash_init(self.hashtype)

    def update(self,data):
        """
.. method:: update(data)

    Update the sha object with the string *data*. Repeated calls are equivalent to a single call with the concatenation of all
    the arguments: m.update(a); m.update(b) is equivalent to m.update(a+b).
        """
        self.ctx = __hash_update(self.hashtype,self.ctx,data)

    def digest(self):
        """
.. method:: digest()

    Return the digest of the strings passed to the update method so far. This is a byte object with length depending on
    the SHA3 variant.
        """
        return __hash_digest(self.hashtype,self.ctx)

    def hexdigest(self):
        """
.. method:: hexdigest()

    Like digest except the digest is returned as a string containing only hexadecimal digits.
        """
        return "".join([hex(x,"") for x in self.digest()])

    def _cctx(self):
        return __make_context(self.hashtype,self.ctx)
    

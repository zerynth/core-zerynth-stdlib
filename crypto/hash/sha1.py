"""
.. module: sha1

****************
SHA1 secure hash
****************

This module implements the interface to NIST’s secure hash algorithm, known as SHA-1. SHA-1 is an improved version of
the original SHA hash algorithm. It is used in the same way as all crypto hash modules: an instance of the SHA class is
created, then feed this object with arbitrary strings/bytes using the update() method, and at any point you can ask it for the digest of the
concatenation of the strings fed to it so far.

The module is based on the C library `cifra <https://github.com/ctz/cifra>`_.
    """

@c_native("zs_sha1_init",["csrc/sha1_ifc.c","csrc/cifra/src/sha1.c","csrc/cifra/src/blockwise.c","csrc/cifra/src/sha1.c"],[],["-I.../csrc/cifra/src","-I.../csrc/cifra/src/ext"])
def __hash_init():
    pass

@c_native("zs_sha1_update",[])
def __hash_update(ctx,data):
    pass

@c_native("zs_sha1_digest",[])
def __hash_digest(ctx):
    pass

@c_native("zs_sha1_context",[])
def __make_context(ctx):
    pass


class SHA1():
    """
==============
The SHA1 class
==============
    
.. class:: SHA1

       This class allows the generation of SHA1 hashes. It is thread safe.
    """
    def __init__(self):
        self.ctx = __hash_init()

    def update(self,data):
        """
.. method:: update(data)

    Update the sha object with the string *data*. Repeated calls are equivalent to a single call with the concatenation of all
    the arguments: m.update(a); m.update(b) is equivalent to m.update(a+b).
        """
        self.ctx = __hash_update(self.ctx,data)

    def digest(self):
        """
.. method:: digest()

    Return the digest of the strings passed to the update method so far. This is a 20-byte bytes object.
        """
        return __hash_digest(self.ctx)

    def hexdigest(self):
        """
.. method:: hexdigest()

    Like digest except the digest is returned as a string of length 40, containing only hexadecimal digits.
        """
        return "".join([hex(x,"") for x in self.digest()])
    
    def _cctx(self):
        return __make_context(self.ctx)

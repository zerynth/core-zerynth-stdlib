"""
.. module: md5

***************
MD5 secure hash
***************

This module implements the interface to MD5 secure hash algorithm.
It is used in the same way as all crypto hash modules: an instance of the MD5 class is
created, then feed this object with arbitrary strings/bytes using the update() method, and at any point you can ask it for the digest of the
concatenation of the strings fed to it so far.

    """

@c_native("zs_md5_init",["csrc/md5_ifc.c"],[],[])
def __hash_init():
    pass

@c_native("zs_md5_update",[])
def __hash_update(ctx,data):
    pass

@c_native("zs_md5_digest",[])
def __hash_digest(ctx):
    pass

@c_native("zs_md5_context",[])
def __make_context(ctx):
    pass


class MD5():
    """
=============
The MD5 class
=============
    
.. class:: MD5

       This class allows the generation of MD5 hashes. It is thread safe.
     """
    def __init__(self):
        self.ctx = __hash_init()

    def update(self,data):
        """
.. method:: update(data)

    Update the md5 object with the string *data*. Repeated calls are equivalent to a single call with the concatenation of all
    the arguments: m.update(a); m.update(b) is equivalent to m.update(a+b).
        """
        self.ctx = __hash_update(self.ctx,data)

    def digest(self):
        """
.. method:: digest()

    Return the digest of the strings passed to the update method so far. This is a 16-byte bytes object.
        """
        return __hash_digest(self.ctx)

    def hexdigest(self):
        """
.. method:: hexdigest()

    Like digest except the digest is returned as a string of length 32, containing only hexadecimal digits.
        """
        return "".join([hex(x,"") for x in self.digest()])   
    
    def _cctx(self):
        return __make_context(self.ctx)

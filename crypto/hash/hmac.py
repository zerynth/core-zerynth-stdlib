"""
.. module:: hmac
   
****
HMAC
****

This module implements the HMAC algorithm as described by `RFC 2104 <https://tools.ietf.org/html/rfc2104.html>`_.

The module is based on the C library `cifra <https://github.com/ctz/cifra>`_.

    """

@c_native("zs_hmac_init",["csrc/hmac_ifc.c","csrc/cifra/src/hmac.c","csrc/cifra/src/blockwise.c","csrc/cifra/src/chash.c"],[],["-I.../csrc/cifra/src","-I.../csrc/cifra/src/ext"])
def __hmac_init(key,hashfn):
    pass

@c_native("zs_hmac_update",[])
def __hmac_update(ctx,data):
    pass

@c_native("zs_hmac_digest",[])
def __hmac_digest(ctx):
    pass


class HMAC():
    """
.. class:: HMAC(key,hashfn)
    
    Return a new hmac object. *key* is a bytes or bytearray or string object giving the secret key. *hashfn* is an
    instance of a hash function to use in hmac generation. It supports any class in the :samp:`crypto.hash` module.
    """
    def __init__(self,key,hashfn):
        self.ctx = __hmac_init(key,hashfn._cctx())

    def update(self,data):
        """
.. method:: update(data)

    Update the hmac object with the string *data*. Repeated calls are equivalent to a single call with the concatenation of all
    the arguments: m.update(a); m.update(b) is equivalent to m.update(a+b).
        """
        self.ctx = __hmac_update(self.ctx,data)

    def digest(self):
        """
.. method:: digest()

    Return the digest of the strings passed to the update method so far. The size depends on *hashfn*.
        """
        return __hmac_digest(self.ctx)

    def hexdigest(self):
        """
.. method:: hexdigest()

    Like digest except the digest is returned as a string containing only hexadecimal digits.
        """
        return "".join([hex(x,"") for x in self.digest()])


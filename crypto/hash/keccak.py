"""
.. module: keccak


******************
Keccak secure hash
******************

This module implements the interface to `Keccak <https://keccak.team/index.html>`_ secure hash functions. 
There is a lot of confusion about the relation between Keccak hash functions and SHA3 hash functions.
The NIST organized a contest to develop the next standard secure hash function to use in place of SHA2. The Keccak team won the competition
with its reference implementation known as Keccak secure hash. However the NIST changed the Keccak implementation before making it a standard known as SHA3
in 2015. The standard SHA3, described in `FIPS202 <http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf>`_ is therefore different from the original Keccak (by the value of the padding bytes). Even if SHA3 is now the standard, Keccak hash is still important because, since the contest lasted from 2008 to 2015, in that time many software projects recognized
the enhanced security of Keccak and included the original implementation before the standard was published. One of such project is the blockchain `Ethereum <https://en.wikipedia.org/wiki/Ethereum>`_ . Sadly, both Keccak and official SHA3 are often simply called SHA3, arising much confusion. One simple way to test an implementation of a SHA3 algorithm is to 
generate the hash of the empty string:

    * :samp:`c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470` is the original Keccak-256
    * :samp:`a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a` is the NIST SHA3-256

The module is used in the same way as all crypto hash modules: an instance of the Keccak class is
created, then feed this object with arbitrary strings/bytes using the update() method, and at any point you can ask it for the digest of the
concatenation of the strings fed to it so far.

The class supports 4 variants of Keccak, selectable in the constructor with one of the following constants:

    * :samp:`KECCAK224`
    * :samp:`KECCAK256`
    * :samp:`KECCAK284`
    * :samp:`KECCAK512`

The module is based on a patched version of the C library `cifra <https://github.com/ctz/cifra>`_.

"""

@c_native("zs_keccak_init",["csrc/keccak_ifc.c","csrc/cifra/src/sha3.c","csrc/cifra/src/blockwise.c"],[],["-I.../csrc/cifra/src","-I.../csrc/cifra/src/ext"])
def __hash_init(hashtype):
    pass

@c_native("zs_keccak_update",[])
def __hash_update(hashtype,ctx,data):
    pass

@c_native("zs_keccak_digest",[])
def __hash_digest(hashtype,ctx):
    pass

@c_native("zs_keccak_context",[])
def __make_context(hashtype,ctx):
    pass



KECCAK224 = 0
KECCAK256 = 1
KECCAK384 = 2
KECCAK512 = 3


class Keccak():
    """
================
The Keccak class
================
    
.. class:: Keccak(hashtype=KECCAK256)

       This class allows the generation of Keccak hashes. It is thread safe. By default, it calculates the Keccak-256 variant.
       This behaviour can be changed by passing a different value for *hashtype*
    """
    def __init__(self,hashtype=KECCAK256):
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
    the Keccak variant.
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
    

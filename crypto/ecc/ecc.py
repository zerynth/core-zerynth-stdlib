"""
.. module:: ecc

***************************
Elliptic Curve Cryptography
***************************

This module offer cryptographic primitives based on Elliptic Curves. In particular it provides key generation and validation, signing, and verifying, for the following curves:

* secp160r1 
* secp192r1 (NISTP192)
* secp224r1 (NISTP224)
* secp256r1 (NISTP256)
* secp256k1 (used by Bitcoin)

For an awesome introduction to ECC check `here <https://www.johannes-bauer.com/compsci/ecc/>`_. 
For an online ECC calculator check `here <http://extranet.cryptomathic.com/ecc/index>`_

The module is based on `MicroECC <https://github.com/kmackay/micro-ecc>`_ patched with functions to enable public key recovery (mainly for blockchain applications).

The module defines the following constants defining curves:

    * :samp:`SECP160R1`
    * :samp:`SECP192R1`
    * :samp:`SECP224R1`
    * :samp:`SECP256R1`
    * :samp:`SECP256K1`


    """

SECP160R1=0
SECP192R1=1
SECP224R1=2
SECP256R1=3
SECP256K1=4

@c_native("zc_curve_generate_keys",[
    "csrc/microecc/uECC.c",
    "csrc/microecc/ecc_ifc.c",
    ],["uECC_SUPPORTS_secp160r1","uECC_SUPPORTS_secp192r1","uECC_SUPPORTS_secp224r1","uECC_SUPPORTS_secp256r1","uECC_SUPPORTS_secp256k1"],["-I.../../hash/csrc"])
def make_keys(curve):
    """
.. function:: make_keys(curve)

    Return a tuple of two elements. The first element is a byte object
    containing the uncompressed representation of the generated public key. The
    second element is a byte object containing the representation of the
    generated public key. *curve* specifies the curve to use

    This function uses the random number generator provided by the
    VM. For real world usage and enhanced security the random number generator
    must be of cryptographic quality (generally implemented in hardware).

    """
    pass

@c_native("zc_curve_check_pbkey",[])
def check_public_key(curve,pbkey):
    """
.. function:: check_public_key(curve,pbkey)

    Return :samp:`True` if *pbkey* (in uncompressed format) is a valid public
    key for *curve*.
    
    """
    pass

@c_native("zc_curve_derive_pbkey",[])
def derive_public_key(curve,pvkey):
    """
.. function:: derive_public_key(curve,pvkey)
    
    Return a byte object containing the uncompressed representation of the
    public key matching *pvkey* for curve *curve*.

    Raise :samp:`ValueError` if derivation is not possible.
    
    """

    pass

@c_native("zc_curve_compress_key",[])
def compress_key(curve,key):
    """
.. function:: compress_key(curve, key)
    
    Return a compressed representation of *key*.
    
    """
    pass

@c_native("zc_curve_decompress_key",[])
def decompress_key(curve,key):
    """
.. function:: decompress_key(curve, key)
    
    Return a uncompressed representation of *key*.
    
    """
    pass


@c_native("zc_curve_verify",[])
def verify(curve,message,signature, pbkey):
    """
.. function:: verify(curve, message, signature, pbkey)
    
    Return :samp:`True` if *signature* is a valid signature for message
    *message* given *curve* and a public key *pbkey*. 
    
    """

    pass

@c_native("zc_curve_sign",[])
def _csign(curve,message,pvkey,deterministic):
    pass

def sign(curve,message,pvkey,deterministic = False, recoverable = False):
    """
.. function:: sign(curve, message, pvkey, deterministic = False, recoverable = False)
    
    Return the signature of *message* with *pvkey* for curve *curve*. Usually
    the message to sign is not the entire message but a hash of it. The
    *deterministic* parameter, if given, creates a deterministic signature
    according to `RFC6979 <https://tools.ietf.org/html/rfc6979>`_ . If given, the *deterministic* parameter must be an
    instance of a hash class from module :samp:`crypto.hash`. Deterministic signatures are not dependent on a good
    random number generator for their security and can therefore be used in hardware without such capabilities. If *recoverable* is given and True, the returned object is a tuple such that the first element is the recovery id and the second element is the signature. The recovery id is a parameter that can be used to derive the public key from a just a valid signature. For more info refer to `this paper <www.secg.org/sec1-v2.pdf>`_.
    
    """
    if deterministic:
        hashctx = deterministic._cctx()
    else:
        hashctx = ""
    cs = _csign(curve,message,pvkey,hashctx)
    if recoverable:
        return cs
    else:
        return cs[1]

def bin_to_hex(key):
    return "".join([hex(x,"") for x in key])

def hex_to_bin(key,skip_zero=False):
    lk = len(key)
    if lk%2==1:
        key="0"+key
        lk+=1
    ret = bytearray(lk//2)
    for i in range(0,lk,2):
        k = int(key[i:i+2],16)
        ret[i//2]=k
    # if skip_zero:
    #     while ret.startswith(b'\x00'):
    #         ret = ret[1:]
    return ret

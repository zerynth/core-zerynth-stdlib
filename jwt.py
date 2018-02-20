# -*- coding: utf-8 -*-
# @Author: lorenzo
# @Date:   2017-10-12 17:03:35
# @Last Modified by:   Lorenzo
# @Last Modified time: 2017-10-26 11:03:22

"""
.. module:: 

***************
JSON Web Tokens
***************


This module allows handling `JSON Web Tokens <https://tools.ietf.org/html/rfc7519>`_ from Zerynth programs.

    """

import base64
from crypto.hash import sha2 as sha2
from crypto.ecc import ecc as ec

# The standard JWT header already base64 encoded. Equates to {"alg": "ES256", "typ": "JWT"}
jwt_header = "eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiJ9";

def encode(payload, key):
    """
.. function:: encode(payload, key)

    Encode a JWT for target :samp:`payload` signed with :samp:`key`.

    Currently only ES256 encoding algorithm using prime256v1 curve is supported.

    :samp:`key` must be an ECDSA private key in hex format.
    If a private key is, for example, stored as a pem file, the needed hex string can be extracted from the OCTET STRING field associated value obtained from ::

        openssl asn1parse -in my_private.pem
        
    command (since pem is a base64 encoded, plus header, `DER <https://tools.ietf.org/html/rfc5915>`_).
    """
    key = ec.hex_to_bin(key)
    jwt = jwt_header + '.' + base64.standard_b64encode(payload).strip('=')
    ss = sha2.SHA2(hashtype=sha2.SHA256)
    ss.update(jwt)
    signature = ec.sign(ec.SECP256R1, ss.digest(), key, deterministic=sha2.SHA2(hashtype=sha2.SHA256))
    jwt = jwt + '.' + base64.standard_b64encode(signature).strip('=')
    return jwt

.. module:: 

***************
JSON Web Tokens
***************


This module allows handling `JSON Web Tokens <https://tools.ietf.org/html/rfc7519>`_ from Zerynth programs.

    
.. function:: encode(payload, key)

    Encode a JWT for target :samp:`payload` signed with :samp:`key`.

    Currently only ES256 encoding algorithm using prime256v1 curve is supported.

    :samp:`key` must be an ECDSA private key in hex format.
    If a private key is, for example, stored as a pem file, the needed hex string can be extracted from the OCTET STRING field associated value obtained from ::

        openssl asn1parse -in my_private.pem
        
    command (since pem is a base64 encoded, plus header, `DER <https://tools.ietf.org/html/rfc5915>`_).
    

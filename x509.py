# -*- coding: utf-8 -*-
# @Author: Lorenzo
# @Date:   2018-05-30 10:13:28
# @Last Modified by:   Lorenzo
# @Last Modified time: 2018-06-27 17:19:11

"""
.. module:: x509
   :synopsis: Handle x509 certificates


****
X509
****

This module allows to perform useful operations involving `x509 certificates <x509wiki_>`_.


.. _x509wiki : https://en.wikipedia.org/wiki/X.509

    """

@native_c("_generate_csr_for_key",
    [
        "csrc/x509/x509_ifc.c",
        "#csrc/misc/zstdlib.c",
        "#csrc/hwcrypto/*",
#-if !HAS_BUILTIN_MBEDTLS
        # compile whole mbedtls library except for net_sockets.c since socks are not needed
        "#csrc/tls/mbedtls/library/pk_wrap.c",
        "#csrc/tls/mbedtls/library/pem.c",
        "#csrc/tls/mbedtls/library/aes.c",
        "#csrc/tls/mbedtls/library/memory_buffer_alloc.c",
        "#csrc/tls/mbedtls/library/ecdsa.c",
        "#csrc/tls/mbedtls/library/asn1parse.c",
        "#csrc/tls/mbedtls/library/ssl_ticket.c",
        "#csrc/tls/mbedtls/library/dhm.c",
        "#csrc/tls/mbedtls/library/asn1write.c",
        "#csrc/tls/mbedtls/library/ccm.c",
        "#csrc/tls/mbedtls/library/x509write_crt.c",
        "#csrc/tls/mbedtls/library/xtea.c",
        "#csrc/tls/mbedtls/library/sha256.c",
        "#csrc/tls/mbedtls/library/x509_crl.c",
        "#csrc/tls/mbedtls/library/ssl_cache.c",
        "#csrc/tls/mbedtls/library/ssl_ciphersuites.c",
        "#csrc/tls/mbedtls/library/pkcs12.c",
        "#csrc/tls/mbedtls/library/ecdh.c",
        "#csrc/tls/mbedtls/library/rsa.c",
        "#csrc/tls/mbedtls/library/debug.c",
        "#csrc/tls/mbedtls/library/camellia.c",
        "#csrc/tls/mbedtls/library/gcm.c",
        "#csrc/tls/mbedtls/library/error.c",
        "#csrc/tls/mbedtls/library/x509write_csr.c",
        "#csrc/tls/mbedtls/library/md5.c",
        "#csrc/tls/mbedtls/library/pkwrite.c",
        "#csrc/tls/mbedtls/library/cipher_wrap.c",
        "#csrc/tls/mbedtls/library/des.c",
        "#csrc/tls/mbedtls/library/ripemd160.c",
        "#csrc/tls/mbedtls/library/cmac.c",
        "#csrc/tls/mbedtls/library/entropy.c",
        "#csrc/tls/mbedtls/library/ssl_tls.c",
        "#csrc/tls/mbedtls/library/md_wrap.c",
        "#csrc/tls/mbedtls/library/pkparse.c",
        "#csrc/tls/mbedtls/library/ssl_cookie.c",
        "#csrc/tls/mbedtls/library/arc4.c",
        "#csrc/tls/mbedtls/library/entropy_poll.c",
        "#csrc/tls/mbedtls/library/cipher.c",
        "#csrc/tls/mbedtls/library/pkcs5.c",
        "#csrc/tls/mbedtls/library/sha512.c",
        "#csrc/tls/mbedtls/library/x509_crt.c",
        "#csrc/tls/mbedtls/library/threading.c",
        "#csrc/tls/mbedtls/library/ssl_srv.c",
        "#csrc/tls/mbedtls/library/base64.c",
        "#csrc/tls/mbedtls/library/version.c",
        "#csrc/tls/mbedtls/library/blowfish.c",
        "#csrc/tls/mbedtls/library/hmac_drbg.c",
        "#csrc/tls/mbedtls/library/havege.c",
        "#csrc/tls/mbedtls/library/x509_csr.c",
        "#csrc/tls/mbedtls/library/oid.c",
        "#csrc/tls/mbedtls/library/x509_create.c",
        "#csrc/tls/mbedtls/library/aesni.c",
        "#csrc/tls/mbedtls/library/certs.c",
        "#csrc/tls/mbedtls/library/pk.c",
        "#csrc/tls/mbedtls/library/bignum.c",
        "#csrc/tls/mbedtls/library/ctr_drbg.c",
        "#csrc/tls/mbedtls/library/timing.c",
        "#csrc/tls/mbedtls/library/ecjpake.c",
        "#csrc/tls/mbedtls/library/md4.c",
        "#csrc/tls/mbedtls/library/md.c",
        "#csrc/tls/mbedtls/library/version_features.c",
        "#csrc/tls/mbedtls/library/sha1.c",
        "#csrc/tls/mbedtls/library/padlock.c",
        "#csrc/tls/mbedtls/library/md2.c",
        "#csrc/tls/mbedtls/library/ssl_cli.c",
        "#csrc/tls/mbedtls/library/x509.c",
        "#csrc/tls/mbedtls/library/ecp.c",
        "#csrc/tls/mbedtls/library/platform.c",
        "#csrc/tls/mbedtls/library/ecp_curves.c",
#-endif
    ],
    [
#-if HAS_BUILTIN_MBEDTLS
#include custom mbedtls headers
    "VHAL_MBEDTLS"
#-endif
    ],
    [
        "-I#csrc/hwcrypto",
#-if !HAS_BUILTIN_MBEDTLS
        "-I#csrc/tls/mbedtls/include"
#-endif
    ])
def generate_csr_for_key(key, subject):
    """
.. function:: generate_csr_for_key(key, subject)

Generate a Certificate Signing Request from :samp:`subject` signed with key :samp:`key`.

    :param subject: is a string containing a comma-separated list of OID types and values (e.g. ``"C=IT,O=ZER,CN=device 1"``)
    :param key: can be: a null terminated string (must end with a 0 byte) containing a valid key or an empty string to use a hardware one.

To enable the use of hardware keys a hardware cryptographic interface must be started.

For example, with an ``ATECC508A`` (:ref:`ATECCx08A interface <lib.microchip.ateccx08a.hwcryptointerface>`): ::

    import x509
    from microchip.ateccx08a import ateccx08a

    # ...

    ateccx08a.hwcrypto_init(I2C0, 0) # select private key stored in slot 0
    x509.generate_csr_for_key('', subject)
    
(in this case ``ZERYNTH_HWCRYPTO_ATECCx08A`` must be also set to true in ``project.yml``)

.. note:: generating a CSR requires a big stack, probably not available inside the main thread. It is suggested to call this function from a separate thread with a custom stack size (e.g. :code:`thread(generate_function, size=12288)`) .

    """
    pass

.. module:: x509
   :synopsis: Handle x509 certificates


****
X509
****

This module allows to perform useful operations involving `x509 certificates <x509wiki_>`_.


.. _x509wiki : https://en.wikipedia.org/wiki/X.509

    
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

    

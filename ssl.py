"""
.. stdlib.ssl:

.. module:: ssl

*******************
Secure Socket Layer
*******************

This module provides access to an almost complete Secure Socket Layer interface. However, some behaviour may be
dependent on the underlying network driver.


SSL/TLS  (TLS from now on) provides encryption and peer authentication facilities for network sockets, both client-side and server-side.
The properties enforced by TLS on data transmission are:

* *confidentiality*: it means the transmitted data is readable only to the two end points of the TLS, it is unreadable garbage for everyon else intercepting the such data during transmission
* *integrity*: it means each end point receives exactly what has been sent by the other end point, no data tampering is possible
* *authentication*: it means each end point can have guarantees on the identity of the other end point

In order to achieve such capabilities, TLS exploits the properties of different cryptographic primitives. 
Confidentiality is obtained by exchanging a symmetric key (a secret known only by he two end points for the duration of the connection) by means of asymmetric key cryptography (usually Diffie-Hellman or RSA key exchange). Such symmetric key will be used to encrypt every transmitted message.
Integrity is guaranteed by appending to transmitted messages a signed digest (called HMAC) that can be generated only by someone knowing the symmetric key.

Authentication is more complex and configurable. Indeed authentication in TLS can be:

    * skipped: the two end points are not interested in the authentication property, therefore they do not require the other end to authenticate at the beginning of the connection
    * one-way: the client can require the server to show some credentials (called certificate) that guarantees its identity. The client can close the connection if the verification of the server certificate fails.
    * two-way: both server and client ask each other a proof of identity by exchanging and verifying certificates.
    
A certificate consists of a public key together with some more data (such as the server domain, the expiration date, etc...) and to guarantee the certificate is not tampered,
it is signed with the private key of a certification authority (CA). The CA public key must be known and it is contained in a set of CA certificates common to both end points.

The mechanism of certificate verification consists of an exchange of asymmetrically crypted messages that proves the identity of the server (and of the client if requested).

No TLS connection is secure without athentication! Therefore, at least a CA certificate must be given to the client in order to authenticate the server and certificate verification must be enabled (it is disabled by default).

Since there are many certification authorities, shipping all CA certificates inside a microcontroller is not feasible. Zerynth provides the useful :samp:`__lookup` macro that can be used to retrieve a single CA certificate
and store it directly in the firmware: ::

    cacert0 = __lookup(SSL_CACERT_COMODO_TRUSTED_SERVICES_ROOT)
    cacert1 = __lookup(SSL_CACERT_DIGICERT_TRUSTED_ROOT_G4)

    # cacert0 will be set to "Comodo Trusted Services" CA certificate
    # cacert1 will be set to "DigiCert Trusted Root G4" CA certificate


To check the TLS feature available in a particular network driver, refer to the following example (based on ESP32): ::

    
    import json
    # import the wifi interface
    from wireless import wifi
    # import the http module
    import requests
    # import ssl module
    import ssl

    # the wifi module needs a networking driver to be loaded
    # in order to control the board hardware.
    from espressif.esp32net import esp32wifi as wifi_driver


    streams.serial()

    # init the wifi driver!
    # The driver automatically registers itself to the wifi interface
    # with the correct configuration for the selected board
    wifi_driver.auto_init()


    # use the wifi interface to link to the Access Point
    # change network name, security and password as needed
    print("Establishing Link...")
    try:
        # FOR THIS EXAMPLE TO WORK, "Network-Name" AND "Wifi-Password" MUST BE SET
        # TO MATCH YOUR ACTUAL NETWORK CONFIGURATION
        wifi.link("Network-Name",wifi.WIFI_WPA2,"Wifi-Password")
    except Exception as e:
        print("ooops, something wrong while linking :(", e)
        while True:
            sleep(1000)

    # let's try to connect to https://www.howsmyssl.com/a/check to get some info
    # on the SSL/TLS connection

    # retrieve the CA certificate used to sign the howsmyssl.com certificate
    cacert = __lookup(SSL_CACERT_DST_ROOT_CA_X3)

    # create a SSL context to require server certificate verification
    ctx = ssl.create_ssl_context(cacert=cacert,options=ssl.CERT_REQUIRED|ssl.SERVER_AUTH)
            
    for i in range(3):
        try:
            print("Trying to connect...")
            url="https://www.howsmyssl.com/a/check"
            # url resolution and http protocol handling are hidden inside the requests module
            user_agent = {"User-Agent": "curl/7.53.1", "Accept": "*/*" }
            # pass the ssl context together with the request
            response = requests.get(url,headers=user_agent,ctx=ctx)
            # if we get here, there has been no exception, exit the loop
            break
        except Exception as e:
            print(e)


    try:
        # check status and print the result
        if response.status==200:
            print("Success!!")
            print("-------------")
            # it's time to parse the json response
            js = json.loads(response.content)
            # super easy!
            for k,v in js.items():
                if k=="given_cipher_suites":
                    print("Supported Ciphers")
                    for cipher in v:
                        print(cipher)
                    print("-----")
                else:
                    print(k,"::",v)
            print("-------------")
    except Exception as e:
        print("ooops, something very wrong! :(",e)




The following CA certificates are available with the __lookup primitive: 

* *Zerynth IoT CA*: ZERYNTH_IOT_CA
* *Zerynth IoT Test CA*: ZERYNTH_IOT_TEST_CA
* *Zerynth IoT Stage CA*: ZERYNTH_IOT_STAGE_CA
* *ACCVRAIZ1*: ACCVRAIZ1
* *AC RAIZ FNMT-RCM*: AC_RAIZ_FNMT_RCM
* *Actalis Authentication Root CA*: ACTALIS_AUTHENTICATION_ROOT_CA
* *AddTrust External Root*: ADDTRUST_EXTERNAL_ROOT
* *AffirmTrust Commercial*: AFFIRMTRUST_COMMERCIAL
* *AffirmTrust Networking*: AFFIRMTRUST_NETWORKING
* *AffirmTrust Premium*: AFFIRMTRUST_PREMIUM
* *AffirmTrust Premium ECC*: AFFIRMTRUST_PREMIUM_ECC
* *Amazon Root CA 1*: AMAZON_ROOT_CA_1
* *Amazon Root CA 2*: AMAZON_ROOT_CA_2
* *Amazon Root CA 3*: AMAZON_ROOT_CA_3
* *Amazon Root CA 4*: AMAZON_ROOT_CA_4
* *Atos TrustedRoot 2011*: ATOS_TRUSTEDROOT_2011
* *Autoridad de Certificacion Firmaprofesional CIF A62634068*: AUTORIDAD_DE_CERTIFICACION_FIRMAPROFESIONAL_CIF_A62634068
* *Baltimore CyberTrust Root*: BALTIMORE_CYBERTRUST_ROOT
* *Buypass Class 2 Root CA*: BUYPASS_CLASS_2_ROOT_CA
* *Buypass Class 3 Root CA*: BUYPASS_CLASS_3_ROOT_CA
* *CA Disig Root R2*: CA_DISIG_ROOT_R2
* *Certigna*: CERTIGNA
* *Certigna Root CA*: CERTIGNA_ROOT_CA
* *certSIGN ROOT CA*: CERTSIGN_ROOT_CA
* *Certum Trusted Network CA 2*: CERTUM_TRUSTED_NETWORK_CA_2
* *Certum Trusted Network CA*: CERTUM_TRUSTED_NETWORK_CA
* *CFCA EV ROOT*: CFCA_EV_ROOT
* *Chambers of Commerce Root - 2008*: CHAMBERS_OF_COMMERCE_ROOT___2008
* *Comodo AAA Services root*: COMODO_AAA_SERVICES_ROOT
* *COMODO Certification Authority*: COMODO_CERTIFICATION_AUTHORITY
* *COMODO ECC Certification Authority*: COMODO_ECC_CERTIFICATION_AUTHORITY
* *COMODO RSA Certification Authority*: COMODO_RSA_CERTIFICATION_AUTHORITY
* *Cybertrust Global Root*: CYBERTRUST_GLOBAL_ROOT
* *DigiCert Assured ID Root CA*: DIGICERT_ASSURED_ID_ROOT_CA
* *DigiCert Assured ID Root G2*: DIGICERT_ASSURED_ID_ROOT_G2
* *DigiCert Assured ID Root G3*: DIGICERT_ASSURED_ID_ROOT_G3
* *DigiCert Global Root CA*: DIGICERT_GLOBAL_ROOT_CA
* *DigiCert Global Root G2*: DIGICERT_GLOBAL_ROOT_G2
* *DigiCert Global Root G3*: DIGICERT_GLOBAL_ROOT_G3
* *DigiCert High Assurance EV Root CA*: DIGICERT_HIGH_ASSURANCE_EV_ROOT_CA
* *DigiCert Trusted Root G4*: DIGICERT_TRUSTED_ROOT_G4
* *DST Root CA X3*: DST_ROOT_CA_X3
* *D-TRUST Root Class 3 CA 2 2009*: D_TRUST_ROOT_CLASS_3_CA_2_2009
* *D-TRUST Root Class 3 CA 2 EV 2009*: D_TRUST_ROOT_CLASS_3_CA_2_EV_2009
* *EC-ACC*: EC_ACC
* *EE Certification Centre Root CA*: EE_CERTIFICATION_CENTRE_ROOT_CA
* *emSign ECC Root CA - C3*: EMSIGN_ECC_ROOT_CA___C3
* *emSign ECC Root CA - G3*: EMSIGN_ECC_ROOT_CA___G3
* *emSign Root CA - C1*: EMSIGN_ROOT_CA___C1
* *emSign Root CA - G1*: EMSIGN_ROOT_CA___G1
* *Entrust.net Premium 2048 Secure Server CA*: ENTRUST.NET_PREMIUM_2048_SECURE_SERVER_CA
* *Entrust Root Certification Authority*: ENTRUST_ROOT_CERTIFICATION_AUTHORITY
* *Entrust Root Certification Authority - EC1*: ENTRUST_ROOT_CERTIFICATION_AUTHORITY___EC1
* *Entrust Root Certification Authority - G2*: ENTRUST_ROOT_CERTIFICATION_AUTHORITY___G2
* *Entrust Root Certification Authority - G4*: ENTRUST_ROOT_CERTIFICATION_AUTHORITY___G4
* *ePKI Root Certification Authority*: EPKI_ROOT_CERTIFICATION_AUTHORITY
* *E-Tugra Certification Authority*: E_TUGRA_CERTIFICATION_AUTHORITY
* *GDCA TrustAUTH R5 ROOT*: GDCA_TRUSTAUTH_R5_ROOT
* *GeoTrust Global CA*: GEOTRUST_GLOBAL_CA
* *GeoTrust Primary Certification Authority*: GEOTRUST_PRIMARY_CERTIFICATION_AUTHORITY
* *GeoTrust Primary Certification Authority - G2*: GEOTRUST_PRIMARY_CERTIFICATION_AUTHORITY___G2
* *GeoTrust Primary Certification Authority - G3*: GEOTRUST_PRIMARY_CERTIFICATION_AUTHORITY___G3
* *GeoTrust Universal CA*: GEOTRUST_UNIVERSAL_CA
* *GeoTrust Universal CA 2*: GEOTRUST_UNIVERSAL_CA_2
* *Global Chambersign Root - 2008*: GLOBAL_CHAMBERSIGN_ROOT___2008
* *GlobalSign ECC Root CA - R4*: GLOBALSIGN_ECC_ROOT_CA___R4
* *GlobalSign ECC Root CA - R5*: GLOBALSIGN_ECC_ROOT_CA___R5
* *GlobalSign Root CA*: GLOBALSIGN_ROOT_CA
* *GlobalSign Root CA - R2*: GLOBALSIGN_ROOT_CA___R2
* *GlobalSign Root CA - R3*: GLOBALSIGN_ROOT_CA___R3
* *GlobalSign Root CA - R6*: GLOBALSIGN_ROOT_CA___R6
* *Go Daddy Class 2 CA*: GO_DADDY_CLASS_2_CA
* *Go Daddy Root Certificate Authority - G2*: GO_DADDY_ROOT_CERTIFICATE_AUTHORITY___G2
* *GTS Root R1*: GTS_ROOT_R1
* *GTS Root R2*: GTS_ROOT_R2
* *GTS Root R3*: GTS_ROOT_R3
* *GTS Root R4*: GTS_ROOT_R4
* *Hellenic Academic and Research Institutions ECC RootCA 2015*: HELLENIC_ACADEMIC_AND_RESEARCH_INSTITUTIONS_ECC_ROOTCA_2015
* *Hellenic Academic and Research Institutions RootCA 2011*: HELLENIC_ACADEMIC_AND_RESEARCH_INSTITUTIONS_ROOTCA_2011
* *Hellenic Academic and Research Institutions RootCA 2015*: HELLENIC_ACADEMIC_AND_RESEARCH_INSTITUTIONS_ROOTCA_2015
* *Hongkong Post Root CA 1*: HONGKONG_POST_ROOT_CA_1
* *Hongkong Post Root CA 3*: HONGKONG_POST_ROOT_CA_3
* *IdenTrust Commercial Root CA 1*: IDENTRUST_COMMERCIAL_ROOT_CA_1
* *IdenTrust Public Sector Root CA 1*: IDENTRUST_PUBLIC_SECTOR_ROOT_CA_1
* *ISRG Root X1*: ISRG_ROOT_X1
* *Izenpe.com*: IZENPE.COM
* *LuxTrust Global Root 2*: LUXTRUST_GLOBAL_ROOT_2
* *Microsec e-Szigno Root CA 2009*: MICROSEC_E_SZIGNO_ROOT_CA_2009
* *NetLock Arany (Class Gold) Főtanúsítvány*: NETLOCK_ARANY_(CLASS_GOLD)_FŐTANÚSÍTVÁNY
* *Network Solutions Certificate Authority*: NETWORK_SOLUTIONS_CERTIFICATE_AUTHORITY
* *OISTE WISeKey Global Root GA CA*: OISTE_WISEKEY_GLOBAL_ROOT_GA_CA
* *OISTE WISeKey Global Root GB CA*: OISTE_WISEKEY_GLOBAL_ROOT_GB_CA
* *OISTE WISeKey Global Root GC CA*: OISTE_WISEKEY_GLOBAL_ROOT_GC_CA
* *QuoVadis Root CA 1 G3*: QUOVADIS_ROOT_CA_1_G3
* *QuoVadis Root CA 2*: QUOVADIS_ROOT_CA_2
* *QuoVadis Root CA*: QUOVADIS_ROOT_CA
* *QuoVadis Root CA 2 G3*: QUOVADIS_ROOT_CA_2_G3
* *QuoVadis Root CA 3*: QUOVADIS_ROOT_CA_3
* *QuoVadis Root CA 3 G3*: QUOVADIS_ROOT_CA_3_G3
* *Secure Global CA*: SECURE_GLOBAL_CA
* *SecureSign RootCA11*: SECURESIGN_ROOTCA11
* *SecureTrust CA*: SECURETRUST_CA
* *Security Communication Root CA*: SECURITY_COMMUNICATION_ROOT_CA
* *Security Communication RootCA2*: SECURITY_COMMUNICATION_ROOTCA2
* *Sonera Class 2 Root CA*: SONERA_CLASS_2_ROOT_CA
* *SSL.com EV Root Certification Authority ECC*: SSL.COM_EV_ROOT_CERTIFICATION_AUTHORITY_ECC
* *SSL.com EV Root Certification Authority RSA R2*: SSL.COM_EV_ROOT_CERTIFICATION_AUTHORITY_RSA_R2
* *SSL.com Root Certification Authority ECC*: SSL.COM_ROOT_CERTIFICATION_AUTHORITY_ECC
* *SSL.com Root Certification Authority RSA*: SSL.COM_ROOT_CERTIFICATION_AUTHORITY_RSA
* *Staat der Nederlanden EV Root CA*: STAAT_DER_NEDERLANDEN_EV_ROOT_CA
* *Staat der Nederlanden Root CA - G2*: STAAT_DER_NEDERLANDEN_ROOT_CA___G2
* *Staat der Nederlanden Root CA - G3*: STAAT_DER_NEDERLANDEN_ROOT_CA___G3
* *Starfield Class 2 CA*: STARFIELD_CLASS_2_CA
* *Starfield Root Certificate Authority - G2*: STARFIELD_ROOT_CERTIFICATE_AUTHORITY___G2
* *Starfield Services Root Certificate Authority - G2*: STARFIELD_SERVICES_ROOT_CERTIFICATE_AUTHORITY___G2
* *SwissSign Gold CA - G2*: SWISSSIGN_GOLD_CA___G2
* *SwissSign Silver CA - G2*: SWISSSIGN_SILVER_CA___G2
* *SZAFIR ROOT CA2*: SZAFIR_ROOT_CA2
* *Taiwan GRCA*: TAIWAN_GRCA
* *TeliaSonera Root CA v1*: TELIASONERA_ROOT_CA_V1
* *thawte Primary Root CA*: THAWTE_PRIMARY_ROOT_CA
* *thawte Primary Root CA - G2*: THAWTE_PRIMARY_ROOT_CA___G2
* *thawte Primary Root CA - G3*: THAWTE_PRIMARY_ROOT_CA___G3
* *TrustCor ECA-1*: TRUSTCOR_ECA_1
* *TrustCor RootCert CA-1*: TRUSTCOR_ROOTCERT_CA_1
* *TrustCor RootCert CA-2*: TRUSTCOR_ROOTCERT_CA_2
* *Trustis FPS Root CA*: TRUSTIS_FPS_ROOT_CA
* *T-TeleSec GlobalRoot Class 2*: T_TELESEC_GLOBALROOT_CLASS_2
* *T-TeleSec GlobalRoot Class 3*: T_TELESEC_GLOBALROOT_CLASS_3
* *TUBITAK Kamu SM SSL Kok Sertifikasi - Surum 1*: TUBITAK_KAMU_SM_SSL_KOK_SERTIFIKASI___SURUM_1
* *TWCA Global Root CA*: TWCA_GLOBAL_ROOT_CA
* *TWCA Root Certification Authority*: TWCA_ROOT_CERTIFICATION_AUTHORITY
* *UCA Extended Validation Root*: UCA_EXTENDED_VALIDATION_ROOT
* *UCA Global G2 Root*: UCA_GLOBAL_G2_ROOT
* *USERTrust ECC Certification Authority*: USERTRUST_ECC_CERTIFICATION_AUTHORITY
* *USERTrust RSA Certification Authority*: USERTRUST_RSA_CERTIFICATION_AUTHORITY
* *Verisign Class 3 Public Primary Certification Authority - G3*: VERISIGN_CLASS_3_PUBLIC_PRIMARY_CERTIFICATION_AUTHORITY___G3
* *VeriSign Class 3 Public Primary Certification Authority - G4*: VERISIGN_CLASS_3_PUBLIC_PRIMARY_CERTIFICATION_AUTHORITY___G4
* *VeriSign Class 3 Public Primary Certification Authority - G5*: VERISIGN_CLASS_3_PUBLIC_PRIMARY_CERTIFICATION_AUTHORITY___G5
* *VeriSign Universal Root Certification Authority*: VERISIGN_UNIVERSAL_ROOT_CERTIFICATION_AUTHORITY
* *XRamp Global CA Root*: XRAMP_GLOBAL_CA_ROOT


"""

import socket

class sslsocket(socket.socket):
    """
===================
The sslsocket class
===================

.. class:: sslsocket(family=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP, ctx=())
        
        This class represents a secure socket based on SSL/TLS protocol. It inherits from socket.socket
        
        Raise :samp:`IOError` exceptions if socket creation goes wrong.

        The parameter *ctx* is the SSL context to use for the socket. See :function:`create_ssl_context` for details.

        Sockets can be used like this::
        
            # import the ssl module
            import ssl
            # import a module to access a net driver (wifi, eth,...)
            from wireless import wifi
            # import the actual net driver
            from driver.wifi.your_preferred_net_driver import your_preferred_net_driver

            # init the driver
            your_preferred_net_driver.init()

            # link the wifi to an AP
            wifi.link("Your Wifi SSID",WIFI_WPA2,"Your Wifi Password")

            # create a tcp socket
            sock = ssl.sslsocket(type=SOCK_STREAM)

            # connect the socket to net address 192.168.1.10 on port 443
            sock.connect(("192.168.1.10",443))

            # send something on the socket!
            sock.sendall("Hello World!")

    """

    def __init__(self, family=socket.AF_INET, type=socket.SOCK_STREAM, proto=socket.IPPROTO_TCP, ctx=()):
        ch = __default_net["ssl"].secure_socket(family, type, proto, ctx)
        socket.socket.__init__(self, family, type, proto, ch)


CERT_NONE = 1
CERT_OPTIONAL = 2
CERT_REQUIRED = 4
CLIENT_AUTH = 8
SERVER_AUTH = 16


def create_ssl_context(cacert="",clicert="",pkey="",hostname="",options=17):
    """
.. _stdlib.ssl.create_ssl_context

.. function:: create_ssl_context(cacert="",clicert="",pkey="",hostname="",options=ssl.CERT_NONE|ssl.SERVER_AUTH)

    This function generates an SSL context with the following data:

        * **cacert** is the CA certificate that will be used to authenticate the other end of the TLS connection
        * **clicert* is the certificate that the server expects to receive from the client in a two-way TLS authentication
        * **pkey** is the private key matching **clicert**. It can be an empty string to use a hardware stored private key. To enable the use of hardware keys a hardware cryptographic interface must be started. For example, with an ``ATECC508A`` (:ref:`ATECCx08A interface <lib.microchip.ateccx08a.hwcryptointerface>`): ::

            from microchip.ateccx08a import ateccx08a
            # ...
            ateccx08a.hwcrypto_init(I2C0, 0) # select private key stored in slot 0

        (in this case ``ZERYNTH_HWCRYPTO_ATECCx08A`` must be also set to true in ``project.yml``)

        * **hostname** is the hostname expected to match the certificate sent by the server. If not given, the hostname check is skipped
        * **options** is an integer obtained by or'ing together one or more of the following options:

            * :samp:`ssl.CERT_NONE`: no certificate verification is performed (default)
            * :samp:`ssl.CERT_OPTIONAL`: certificate verification is performed only if the other end of the connection sends one, otherwise the certificate is not requested and verification skipped
            * :samp:`ssl.CERT_REQUIRED`: certificate verification is mandatory. If verification fails, :samp:`ConnectionAborted` is raised during TLS handshake.
            * :samp:`ssl.SERVER_AUTH`: indicates that the context may be used to authenticate servers therefore, it will be used to create client-side sockets (default).
            * :samp:`ssl.CLIENT_AUTH`: indicates that the context may be used to authenticate clients therefore, it will be used to create server-side sockets.

    Returns a tuple to be passed as parameter during secure socket creation.

.. note:: **cacert**, **clicert** and **pkey** can be bytes, bytearray, strings or instances of classes that have a **size** and **read** method, allowing to pass as parameters open files or resources.

.. note:: **cacert**, **clicert** and **pkey** must be in PEM format and null-terminated (they must end with a 0 byte).

    """
    if type(cacert)==PINSTANCE:
        #read CA certificate from stream
        sz = cacert.size()
        cacert = cacert.read(sz)
    if type(clicert)==PINSTANCE:
        #read client certificate from stream
        sz = clicert.size()
        clicert = clicert.read(sz)
    if type(pkey)==PINSTANCE:
        #read client pkey from stream
        sz = pkey.size()
        pkey = pkey.read(sz)

    return (cacert,clicert,pkey,hostname,options)



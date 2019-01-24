.. module:: ble

***
BLE
***

This module implements a generic BLE interface for the peripheral role.
To function correctly it needs a BLE driver to be loaded, so that the module can use
the driver to access the underlying hardware.

The link between the BLE module and the BLE driver is established without the programmer
intervention by the driver itself.

The BLE module requires the programmer to understand the specifications of the BLE protocol ragarding in particular the Generic Access Profile (GAP) and the Generic Attribute Profile (GATT).
Official BLE specifications can be found `here <https://www.bluetooth.com/specifications/bluetooth-core-specification>`_. A shorter and easier introduction to BLE can be found `here <https://learn.adafruit.com/introduction-to-bluetooth-low-energy/introduction>`_.

A minimal example follows: ::

    import streams
    # import a BLE driver: in this example we use NRF52
    from nordic.nrf52_ble import nrf52_ble as bledrv
    # then import the BLE modue
    from wireless import ble

    streams.serial()

    # initialize NRF52 driver
    bledrv.init()

    # Set GAP name
    ble.gap("Zerynth")

    # Create a GATT Service: let's try a Battery Service (uuid is 0x180F)
    s = ble.Service(0x180F)

    # Create a GATT Characteristic: (uuid for Battery Level is 0x2A19, and it is an 8-bit number)
    c = ble.Characteristic(0x2A19,ble.NOTIFY | ble.READ,1,"Battery Level",ble.NUMBER)

    # Add the GATT Characteristic to the Service
    s.add_characteristic(c)

    # Add the Service
    ble.add_service(s)

    # Start the BLE stack
    ble.start()

    # Begin advertising
    ble.start_advertising()


    while True:
        print(".")
        sleep(1000)
        # Let's update the Characteristic Value
        c.set_value(random(0,100))


To interact with the example, download a BLE monitor app (for example `this one <https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner&hl=en>`_).





================
Module Functions
================

    
.. function:: gap(name,appearance=0,security=(SECURITY_MODE_1,SECURITY_LEVEL_1),connection=(400,650,0,4000))

        Set parameters for the the Generic Access Profile:

        * :samp:`name` is a string representing the full name of the BLE device. Depending on the advertising mode and payload, the advertised name can be shorter.
        * :samp:`appearance` is a 16-bit number encoding the BLE `appearance <https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml&u=org.bluetooth.characteristic.gap.appearance.xml>`_
        * :samp:`security` is a tuple of integers. The first element is the security mode, the second is the security level. More info `here <https://www.safaribooksonline.com/library/view/getting-started-with/9781491900550/ch04.html>`_ . Constants :samp:`SECURITY_MODE_1` and :samp:`SECURITY_MODE_2` can be used for mode, :samp:`SECURITY_LEVEL_1`, :samp:`SECURITY_LEVEL_2`, :samp:`SECURITY_LEVEL_3` and :samp:`SECURITY_LEVEL_4` for level.
        * :samp:`connection` is a tuple of integers representing connections parameters. The first element specifies the Minimum Connection Interval in milliseconds; the second element specifies the Maximum Connection Interval in milliseconds; the third element specifies the slave latency and it represents the number of times that the peripheral can avoid answering to a central; the fourth element is the maximum time in milliseconds after which a connection is declared lost if no data has been exchanged.

    Security features can be not completely supported by the underlying BLE driver. When supported, the security features are selectable as follows:

    * Security Mode 1: this mode enforces security by means of encryption, and contains four levels

        * Level 1 - No Security (No authentication and no encryption)
        * Level 2 - Unauthenticated pairing with encryption
        * Level 3 - Authenticated pairing with encryption
        * Level 4 - Authenticated LE Secure Connections pairing with encryption

    * Security Mode 2: this mode enforces security by means of data signing, and contains two levels

        * Level 1 - Unauthenticated pairing with data signing
        * Level 2 - Authenticated pairing with data signing

    
.. function:: security(capabilities=CAP_NONE,bonding=AUTH_NO_BOND,scheme=AUTH_SC,key_size=16,initiator=KEY_ENC|KEY_ID,responder=KEY_ENC|KEY_ID,oob=0,passkey=0)
    
        Set security parameters:

        * :samp:`capabilities` is one of the following constants descrbing the device capabilities (to determine how to perform secure pairing):

            * :samp:`CAP_DISPLAY_ONLY`: a device with only a display capability
            * :samp:`CAP_DISPLAY_YES_NO`: a device with a display and the possibility of confirming or unconfirming the displayed passkey (i.e. two buttons)
            * :samp:`CAP_KB_ONLY`: a device with only the possibility of accepting some input
            * :samp:`CAP_KB_DISPLAY`: a device with input and display capabilities
            * :samp:`CAP_NONE`: a device with no input or output capabilities

        * :samp:`bonding` is either AUTH_NO_BOND for pairing without bonding or AUTH_BOND for bonding
        * :samp:`scheme` is an or'ing of the following constants:

            * :samp:`AUTH_MITM` for man in the middle protection
            * :samp:`AUTH_SC` for secure connections

        * :samp:`key_size` between 7 and 16 represents the size of the security keys in bytes
        * :samp:`initiator` determines what kind of keys are exchanged by the initiator:

            * :samp:`KEY_ENC` for the encryption key
            * :samp:`KEY_ID` for the identity key
            * :samp:`KEY_CSR` for the signing key

        * :samp:`responder` same value set of the :samp:`initiator`
        * :samp:`oob`  not supported yet
        * :samp:`passkey` is an integer representing the passkey to be displayed for :samp:`CAP_DISPLAY_ONLY` devices


    
.. function:: bonded()

    Returns the list of addresses of the currently bonded devices.
    Each address is a bytes sequence of length 6

    
.. function:: remove_bonded(addr)

    Removes the bonding with the device with address :samp:`addr`.

    
.. function:: confirm_passkey(confirmed)

    For devices with :samp:`CAP_DISPLAY_YES_NO` the user must confirm the passkey. If :samp:`confirmed` is not zero, the passkey is confirmed and
    the secure connection performed. Otherwise the secure connection is aborted.

    
.. function:: add_service(service)

        Adds :samp:`service` to the service list. :samp:`service` must be an instance of the :class:`.Service` class.
    
.. function:: add_callback(evt,callback)

        Add `callback` to the callback list. It will be triggered by event :samp:`evt`.

        The :samp:`callback` must be a function accepting one positional parameter that will be set to a value specific for each event.
        Only one callback can be linked to a specific event.
        A callback can be linked to GAP events by passing one of the following constants as :samp:`evt`:

        * :samp:`EVT_CONNECTED`, the callback is triggered when a connection is made. Specific value is the remote device address.
        * :samp:`EVT_DISCONNECTED`, the callback is triggered when a connection is lost. Specific value is the remote device address.
        * :samp:`EVT_SCAN_STARTED`, the callback is triggered at the start of a scan.
        * :samp:`EVT_SCAN_STOPPED`, the callback is triggered at the end of a scan.
        * :samp:`EVT_SCAN_REPORT`, the callback is triggered at the end of a scan. Specific value is a tuple containing 5 elements (SCAN_TYPE, ADDR_TYPE, RSSI, PACKET, ADDR).
        * :samp:`EVT_SHOW_PASSKEY`, the callback is triggered when the user need to access the passkey for :samp:`CAP_DISPLAY_ONLY` devices
        * :samp:`EVT_MATCH_PASSKEY`, the callback is triggered when the user need confirm the passkey provided by the central (:samp:`CAP_DISPLAY_YES_NO` devices)
        * :samp:`EVT_AUTH_FAILED` the callback is triggered when the authentication phase fails
        * :samp:`EVT_ADV_STARTED` the callback is triggered when advertising starts
        * :samp:`EVT_ADV_STOPPED` the callback is triggered when advertising stops

        Regarding scan report events, the meaning of the scan tuple is:

        * SCAN_TYPE: an integer assuming values:

            * :samp:`ADV_CONN_UND` for connectable undirected advertising
            * :samp:`ADV_CONN_DIR` for connectable directed advertising
            * :samp:`ADV_SCAN_UND` for scannable undirected advertising
            * :samp:`ADV_UNCN_UND` for non connectable undirected advertising
            * :samp:`ADV_SCAN_RSP` scan response

        * ADDR_TYPE: an integer being 0 for public address, 1 for random address, 2 for public resolvable private address and 3 for random resolvable private address
        * RSSI: the rssi of the scanned device
        * PACKET: a bytes object containing the advertising packet
        * ADDR: a bytes object containing the 48bit device address


        An example of a callback: ::

            from wireless import ble

            def connected(addr):
                print("Connected to",ble.btos(addr))

            ble.add_callback(ble.EVT_CONNECTED,connected)


    
.. function:: advertising(interval,timeout=0, payload="",scan_rsp="",mode=ADV_CONN_UND)

        Set advertising parameters:

        * :samp:`interval` is the time in milliseconds between advertising packets
        * :samp:`timeout` is the time in milliseconds after which stopping the advertising (if zero, it never times out)
        * :samp:`payload` is a string or bytes containing the payload to add in the manufacturer section of the advertising packet. Ignored if len(payload) is zero.
        * :samp:`scanrsp` is a string or bytes containing the payload to add in a scan response. Ignored if len(scanrsp) is zero.
        * :samp:`mode` is an integer determining the type of advertising: :samp:`ADV_CONN_UND`, :samp:`ADV_UNCN_UND`, :samp:`ADV_SCAN_UND` are supported.

    
.. function:: start_advertising()

    Start advertising according to parameters set by :func:`advertising`

    
.. function:: stop_advertising()

    Stop advertising immediately

    
.. function:: scanning(interval=10,window=10,duplicates=0,filter=SCAN_FILTER_ALL,addr=SCAN_ADDR_PUBLIC,active=0)

    Set options for scanning.

    * :samp:`interval` (in milliseconds) is the scanning interval
    * :samp:`window` (in milliseconds) is the duration of a scanning interval
    * :samp:`duplicates` is zero for no duplicate advertisement packets in the report, not zero for duplicates included
    * :samp:`filter` selects which kind of packets to include in the report (not yet implemented)
    * :samp:`addr` selects which kind of device addresses are included in the report (not yet implemented)
    * :samp:`active` selects active scanning if not zero

    
.. function:: start_scanning(duration=1000)

    Start scanning for advertising packets for *duration* milliseconds. Each scanned packet is retrieved int the :samp:`EVT_SCAN_REPORT` GAP event.

    
.. function:: stop_scanning()

    Stop scanning for advertising packets

    
.. function:: start()

    Initializes and start the BLE stack.
    Must be called after the configuration of GAP, GATT, advertising, scanning and security

    
.. function:: btos(addr)

    Converts a BLE address from a sequence of bytes to a string. It can also be used with longer sequences like entire packets.

    
=============
Service class
=============

.. class:: Service(uuid,base_uuid=None)

    This class abstracts a GATT Service. Services are defined by providing a :samp:`uuid`. BLE uuids are 128 bit sequences, but a shorter 16 bit  version can be used in case of standard BLE services.
    The list of standard uuids can be found `here <https://www.bluetooth.com/specifications/gatt/services>`_. For custom uuids it is possible to specify all 128 bits passing a bytes or bytearray object to :samp:`base_uuid`.
    :samp:`base_uuid` represents the 128 bit uuid as a sequece of 16 bytes in little endian order. 13th and 14th bytes are ignored and substituted by :samp:`uuid`.

    To create a standard service: ::

        from wireless import ble
        s = ble.Service(0x180D) # Heart Rate Service id is 0x180D

    To create a custom service ::

        from wireless import ble
        s = ble.Service(0xA001,bytes([0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x00,0x00,0x0F,0x10]))

        # the custom Service uuid becomes: 100FA001-0C0B-0A09-0807-060504030201
        # 13th and 14th bytes are substituted with 0xA001

    
.. method:: add_characteristic(ch)

        Add :samp:`ch` to the list of characteristic contained in the service. :samp:`ch` must be an instance of :class:`.Characteristic`.
        :samp:`ch` is modified by setting :samp:`ch.service` to the service uuid.

        
====================
Characteristic class
====================

.. class:: Characteristic(uuid,permission,size,descriptor,type=NUMBER)

    This class abstracts a GATT Characteristic. Characteristics are defined by providing:

    * :samp:`uuid`: a 16 bit uuid, standard uuids can be found `here <https://www.bluetooth.com/specifications/gatt/characteristics>`_
    * :samp:`permission`: an integer representing permissions available for the characteristic value. It is defined by ORing the following flags:

        * :samp:`READ` for read permission
        * :samp:`WRITE` for write permission
        * :samp:`NOTIFY` for notify permission
        * :samp:`INDICATE` for indication permission

    * :samp:`size`: the length in bytes of the characteristic value
    * :samp:`descriptor`: a string specifying the characteristic descriptor
    * :samp:`type`: can be one of :samp:`NUMBER`, :samp:`STRING`, :samp:`BYTES` and specifies the type of the characteristic value. In BLE specifications, values are sequences of bytes. Using :samp:`size` and :samp:`type`, the BLE module interprets the sequence of bytes transforming it in a Python object.



    To create a characteristic: ::

        from wireless import ble
        c = ble.Characteristic(0x2A19,ble.NOTIFY | ble.READ,1,"Battery Level",ble.NUMBER)

        # The characteristic has uuid 0x2A19, can be read and notified, is a number of size 1 byte and has a descriptor

    Once created, a characteristic must be added to a service to be usable

    
.. method:: set_value(value)

        Update the characteristic value to :samp:`value`. :samp:`value` is converted to bytes sequence according to the characteristic size and type.

        If connected, the NOTIFY permission is given and the central has enabled notifications, the updated value is notified to the central.

        
.. method:: get_value()

        Return the value of the characteristic converted to a Python object according to size and type.

        
.. method:: set_callback(fn)

        Characteristics with WRITE permission can be changed by the central. A callback can be used to be notified of such change.
        :samp:`fn` is called on every WRITE event and accepts two positional arguments; the first one is a status flag, the second one is the updated characteristic value.

        For example: ::

            from wireless import ble

            def led_toggle(status,value):
                if status & ble.WRITE:
                    if value:
                        digitalWrite(LED0,1)
                    else:
                        digitalWrite(LED0,0)

            c = ble.Characteristic(0x1525,ble.WRITE | ble.READ,1,"LED",ble.NUMBER)

            c.set_callback(led_toggle)


        The :samp:`status` argument of the callback is set to a combination of :samp:`READ`, :samp:`NOTIFY`, :samp:`WRITE` events.


        

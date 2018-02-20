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

    
.. function:: gap(name,appearance=0,security=(1,1),connection=(400,650,0,4000))

        Set parameters for the the Generic Access Profile:

        * `name` is a string representing the full name of the BLE device. Depending on the advertising mode and payload, the advertised name can be shorter.
        * `appearance` is a 16-bit number encoding the BLE `appearance <https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml&u=org.bluetooth.characteristic.gap.appearance.xml>`_
        * `security` is a tuple of integers. The first element is the security mode, the second is the security level. More info `here <https://www.safaribooksonline.com/library/view/getting-started-with/9781491900550/ch04.html>`_
        * `connection` is a tuple of integers representing connections parameters. The first element specifies the Minimum Connection Interval in milliseconds; the second element specifies the Maximum Connection Interval in milliseconds; the third element specifies the slave latency and it represents the number of times that the peripheral can avoid answering to a central; the fourth element is the maximum time in milliseconds after which a connection is declared lost if no data has been exchanged.

    
.. function:: add_service(service)

        Adds `service` to the service list. `service` must be an instance of the :class:`.Service`
    
.. function:: add_callback(evt,callback)
    
        Add `callback` to the callback list. It will be triggered by event `evt`.

        A callback can be linked to GAP events by passing one of the following constants as `evt`:

        * `EVT_CONNECTED`, the callback is triggered when a connection is made. Specific value is `None`.
        * `EVT_DISCONNECTED`, the callback is triggered when a connection is lost. Specific value is `None`.
        * `EVT_SCAN_REPORT`, the callback is triggered at the end of a scan. . Specific value is the scanned advertising packet.

        the `callback` must be a function accepting one positional parameter that will be set to a value specific for each event.
        Only one callback can be linked to a specific event.

        For example: ::

            from wireless import ble

            def connected(value):
                print("Connected!")

            ble.add_callback(ble.EVT_CONNECTED,connected)


    
.. function:: advertising(interval,timeout=0,payload="",list_uuids=1)

        Set advertising parameters:

        * `interval` is the time in milliseconds between advertising packets
        * `timeout` is the time in seconds after which stopping the advertising (if zero, it never times out)
        * `payload` is a string or bytes containing the payload to add int he manufacturer section of the advertising packet. Ignored if len(paylod) is zero.
        * `list_uuids` is an integer determining how many service uuids must be added to the advertising packet

    
.. function:: start_advertising()

    Start advertising according to parameters set by :func:`advertising`

    
.. function:: stop_advertising()

    Stop advertising immediately
    
    
.. function:: start_scanning()

    Start scanning for advertising packets
    
    
.. function:: stop_scanning()

    Stop scanning for advertising packets
    
    
.. function:: start()

    Initializes and start the BLE stack.
    Must be called after the configuration of GAP, GATT and advertising.
    
    
=============
Service class
=============

.. class:: Service(uuid,base_uuid=None)

    This class abstracts a GATT Service. Services are defined by providing a `uuid`. BLE uuids are 128 bit sequences, but a shorter 16 bit  version can be used in case of standard BLE services.
    The list of standard uuids can be found `here <https://www.bluetooth.com/specifications/gatt/services>`_. For custom uuids it is possible to specify all 128 bits passing a bytes or bytearray object to `base_uuid`.
    `base_uuid` represents the 128 bit uuid as a sequece of 16 bytes in little endian order. 13th and 14th bytes are ignored and substituted by `uuid`.

    To create a standard service: ::

        from wireless import ble
        s = ble.Service(0x180D) # Heart Rate Service id is 0x180D

    To create a custom service ::

        from wireless import ble
        s = ble.Service(0xA001,bytes([0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x00,0x00,0x0F,0x10]))

        # the custom Service uuid becomes: 100FA001-0C0B-0A09-0807-060504030201
        # 13th and 14th bytes are substituted with 0xA001

    
.. method:: add_characteristic(ch)
        
        Add `ch` to the list of characteristic contained in the service. `ch` must be an instance of :class:`.Characteristic`.
        `ch` is modified by setting `ch.service` to the service uuid.

        
====================
Characteristic class
====================

.. class:: Characteristic(uuid,permission,size,descriptor,type=NUMBER)

    This class abstracts a GATT Characteristic. Characteristics are defined by providing:

    * `uuid`: a 16 bit uuid, standard uuids can be found `here <https://www.bluetooth.com/specifications/gatt/characteristics>`_
    * `permission`: an integer representing permissions available for the characteristic value. It is defined by ORing the following flags:

        * `READ` for read permission
        * `WRITE` for write permission
        * `NOTIFY` for notify permission
    
    * `size`: the length in bytes of the characteristic value
    * `descriptor`: a string specifying the characteristic descriptor
    * `type`: can be one of `NUMBER`,`STRING`,`BYTES` and specifies the type of the characteristic value. In BLE specifications, values are sequences of bytes. Using `size` and `type`, the BLE module interprets the sequence of bytes transforming it in a Python object.



    To create a characteristic: ::

        from wireless import ble
        c = ble.Characteristic(0x2A19,ble.NOTIFY | ble.READ,1,"Battery Level",ble.NUMBER) 

        # The characteristic has uuid 0x2A19, can be read and notified, is a number of size 1 byte and has a descriptor

    Once created, a characteristic must be added to a service to be usable

    
.. method:: set_value(value)
        
        Update the characteristic value to `value`. `value` is converted to bytes sequence according to the characteristic size and type.

        If connected, the NOTIFY permission is given and the central has enabled notifications, the updated value is notified to the central.

        
.. method:: get_value()
        
        Return the value of the characteristic converted to a Python object according to size and type.

        
.. method:: set_callback(fn)
        
        Characteristics with WRITE permission can be changed by the central. A callback can be used to be notified of such change.
        `fn` is called on every WRITE event and accepts two positional arguments; the first one is a status flag, the second one is the updated characteristic value.

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
    

        The `status` argument of the callback is set to a combination of `READ`, `NOTIFY`, `WRITE` events.


        

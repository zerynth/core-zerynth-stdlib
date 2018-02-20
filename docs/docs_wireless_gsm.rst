.. module:: gsm

.. _stdlib_gsm:
***
GSM
***

This module implements a generic gsm interface.
To function correctly it needs a gsm driver to be loaded, so that the module can use
the driver to access the underlying hardware.

The link between the gsm module and the gsm driver is established without the programmer
intervetion by the driver itself.



Once the gsm driver is connected to the GSM network, this module can be used to access functionalities at different levels:

    * **Network level**: query the operators/carriers, select a carrier, retrieve the signal strength, retrieve info about the current cell
    * **Device level**: retrieve info about IMEI and SIM card
    * **Packet Switch Service level**: once attached to an APN, the standard socket interface is available. Secure sockets are available if the underlying driver supports them. Due to the nature of GSM networks (private networks behind a NATing APN), listening sockets are not implemented both for UDP and TCP. 
    * **SMS level**: receive, read and send SMS (not yet available)


The following constants are defined:

    * :samp:`AUTH_NONE`, no authentication required with APN
    * :samp:`AUTH_DETECT`, automatic authentication detection
    * :samp:`AUTH_PAP`, use PAP authentication
    * :samp:`AUTH_CHAP`, use CHAP authentication

    
.. function:: gethostbyname(hostname)

        Translate a host name to IPv4 address format. The IPv4 address is returned as a string, such as "192.168.0.5".

    
.. function:: attach(apn, username = "", psw = "", auth = AUTH_NONE, timeout=120000)

        Try to establish a link with the chosen Access Point Name *apn*.
        *auth* must be one of AUTH_NONE (default), AUTH_PAP, AUTH_CHAP, AUTH_DETECT.
        *username* and *psw* are optional, depending on the operator.
        *timeout* is the time in milliseconds to wait for network registration (connecting to the carrier and receiving GPRS support) and APN negotiation (enabling the packet switch service)

        An exception can be raised if the connection is not successful.

    
.. function:: detach()

        Detach from APN.

    
.. function:: mobile_info()        

        Return information on the G350 device and SIM card.

        The result is a tuple where the elements are, in order:

            * The IMEI as a string
            * The SIM card id as a string

    
.. function:: link_info()        

        Return information on the currently established APN link.

        The result is a tuple where the elements are, in order:

            * The assigned IP as a string
            * The DNS IP as a string

        Note that the returned IP, is in the private range and doesn't match the IP seen by the other end of a socket.
    
.. function:: network_info()        

        Return information on the GSM network.

        The result is a tuple where the elements are, in order:

            * The service type as a string: GMS, UMTS, LTE
            * The `MCC <http://mcc-mnc.com>`_ as an integer, negative if not known
            * the `MNC <http://mcc-mnc.com>`_ as an integer, negative if not known
            * The Base Station identifier as string 
            * The LAC (location area code) as a string
            * The Cell Identity as a string 
            * The registration status to the mobile network: True or False
            * The presence of a link to an APN: True or False

    
.. function:: rssi()        

        Return an integer representing the signal strength in dB. 
    
    
.. function:: operators()

    Retrieve the list of available operators/carrier. If the result is :samp:`None`, it was not possible to retrieve the list of operators.

    The result is a tuple where each items is a tuple of the following items:

        * *type*, being 1 if available and 2 if currently connected
        * *long name*, a string with the full name of the operator
        * *short name*, a string with the short name of the operator
        * *identifier*, the numeric identifier of the operator as a string

    
.. function:: set_operator(opname)

    Try to select the operator with long name *opname*. It is suggested to retrieve the list of operators before trying to set one.

    
.. function:: select(rlist, wlist, xlist, timeout=None)

   This is equivalent to the Unix *select* system call.
   The first three arguments are sequences of socket instances.

   * *rlist*: wait until ready for reading
   * *wlist*: wait until ready for writing
   * *xlist*: wait for an "exceptional condition" (not supported by every gsm driver)

   Empty sequences are allowed. The optional *timeout* argument specifies a time-out as an integer number
   in milliseconds.  When the *timeout* argument is omitted the function blocks until
   at least one socket is ready.  A *timeout* value of zero specifies a
   poll and never blocks.

   The return value is a triple of lists of objects that are ready: subsets of the
   first three arguments.  When the time-out is reached without a socket
   becoming ready, three empty lists are returned.
       
    

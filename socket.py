"""
.. module:: socket

*******
Sockets
*******

This module provides access to an almost complete BSD socket interface. However, some behaviour may be
dependent on the underlying network driver.

The following constants are defined:

    * For socket families: AF_INET, AF_INET6, AF_CAN
    * For socket types: SOCK_STREAM, SOCK_DGRAM, SOCK_RAW 
    * For socket options: SOL_SOCKET, SO_RCVTIMEO

IPv4 addresses can be passed to functions and methods in the following forms:

    * string, e.g. "192.168.1.10"
    * tuple, e.g. (192,168,1,10)
    * tuple of ip and port, e.g. ("192.168.1.10",8080)
    * tuple of ip and port, e.g. (192,168,1,10,8080)

if a port is required but not given, it is set to zero.

    """


AF_INET = 0
AF_INET6 = 1
AF_CAN = 2

SOCK_STREAM = 0
SOCK_DGRAM = 1
SOCK_RAW = 2


SOL_SOCKET = 0xffff
SO_RCVTIMEO = 1
SO_KEEPALIVE = 8

TCP_KEEPIDLE=0x03
TCP_KEEPINTVL=0x04
TCP_KEEPCNT=0x05 

IPPROTO_TCP=6
IPPROTO_UDP=17


# def _address_to_address(address):
#     if type(address)==PSTRING:
#         idx_cl = address.find(__ORD(":"))
#         return net_address(address[0:idx_cl],int(address[idx_cl+1:]))
#     elif type(address)==PSMALLINT:
#         if address<=0 or address>=65536:
#             raise TypeError
#         return net_address(0,0,0,0,address)
#     else:
#         return address


def ip_to_tuple(ip):
    """
    .. method:: ip_to_tuple()

            Return a tuple of four integers from a ip address of the form "x.y.z.w".
    """          
    ipt = ip.split(".")
    return (int(ipt[0]),int(ipt[1]),int(ipt[2]),int(ipt[3]))

#TODO: implement for INET6 and CAN

class socket():
    """
================
The socket class
================

.. class:: socket(family=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP, fileno=None)
        
        This class represents a BSD socket.
        
        Raise :data:`__builtins__.IOError` exceptions if socket creation goes wrong.

        Sockets can be used like this::

            # import the socket module
            import socket
            # import a module to access a net driver (wifi, eth,...)
            from wireless import wifi
            # import the actual net driver
            from driver.wifi.your_preferred_net_driver import your_preferred_net_driver

            # init the driver
            your_preferred_net_driver.init()

            # link the wifi to an AP
            wifi.link("Your Wifi SSID",WIFI_WPA2,"Your Wifi Password")

            # create a tcp socket
            sock = socket.socket(type=SOCK_STREAM)

            # connect the socket to net address 192.168.1.10 on port 5555
            sock.connect(("192.168.1.10",5555))

            # send something on the socket!
            sock.sendall("Hello World!")


    """
    def __init__(self,family=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP, fileno=None):
        self.family = family
        self.netdrv = __default_net["sock"][family]
        self.timeout=None
        if fileno is None:
            if type==SOCK_STREAM:
                self.type = SOCK_STREAM
            else:
                self.type = SOCK_DGRAM
            self.channel = self.netdrv.socket(family,type,proto)
        else:
            self.channel = fileno
            self.type = type

    def fileno(self):
        """
.. method:: fileno()

        Return an integer identifying the underlying socket number.
        """        
        return self.channel

    def connect(self,address):
        """
.. method:: connect(address)

        Tries to connect the underlying socket (tcp or udp) to *address*.
        A tcp socket must be connected to be used successfully. Udp sockets are connectionless and everytime a datagram
        is sent, the receiver address must be specified (:meth:`.sendto`). However if an udp socket is connected to an address,
        it can be used with methods like :meth:`.recv` and :meth:`.send` without specifying a receiver address.
        When an udp socket is connected to *address*, datagram packets coming from addresses different from *address* are ignored.
        
        """
        #address = _address_to_address(address)
        self.netdrv.connect(self.channel,address)

    def close(self):
        """
.. method:: close()

        Closes the underlying socket. No more input/output operations are possible.
        """        
        self.netdrv.close(self.channel)


    def recv(self,bufsize,flags=0):
        """
.. method:: recv(bufsize,flags=0)

        Reads at most *bufsize* bytes from the underlying socket. It blocks until *bufsize* bytes are received or an error occurs.

        Returns a bytearray containing the received bytes.
        """
        buf = bytearray(bufsize)
        rd = self.netdrv.recv_into(self.channel,buf,bufsize,flags)
        __elements_set(buf,rd)
        return buf

    def recv_into(self,buffer, bufsize=-1,flags=0,ofs=0):
        """
.. method:: recv_into(buffer,bufsize=-1,flags=0)

        Reads at most *bufsize* bytes from the underlying socket into *buffer*. It blocks until *bufsize* bytes are received or an error occurs.

        Returns the number of received bytes.
        """
        if bufsize<0:
            bufsize=len(buffer)
        rd = self.netdrv.recv_into(self.channel,buffer,bufsize,flags,ofs)
        return rd


    def recvfrom(self,bufsize,flags=0):
        """
.. method:: recvfrom(bufsize,flags=0)

        Reads at most *bufsize* bytes from the underlying udp socket. It blocks until a datagram is received.

        Returns a tuple (*data*, *address*) where *data* is a bytearray containing the received bytes and *address* is the net address of the sender.
        """
        buf = bytearray(bufsize)
        rd,address = self.netdrv.recvfrom_into(self.channel,buf,bufsize,flags)
        __elements_set(buf,rd)
        return (buf,address)

    def recvfrom_into(self,buffer, bufsize=-1,flags=0):
        """
.. method:: recvfrom_into(buffer,bufsize=-1,flags=0)

        Reads at most *bufsize* bytes from the underlying udp socket into *buffer*. It blocks until a datagram is received. If *bufsize* is not given or is less than 0, *bufsize* is set to len(buffer).

        Returns a tuple (*rd*, *address*) where *rd* is the number of bytes received and *address* is the net address of the sender.
        """
        if bufsize<0:
            bufsize=len(buffer)
        rd,address = self.netdrv.recvfrom_into(self.channel,buffer,bufsize,flags)
        return (rd,address)


    def send(self,buffer,flags=0):
        """
.. method:: send(buffer,flags=0)

        Send data to the socket. The socket must be connected to a remote socket. 

        Returns the number of bytes sent. Applications are responsible for checking that all data has been sent; if only some of the data was transmitted, the application needs to attempt delivery of the remaining data.
        """
        return self.netdrv.send(self.channel,buffer,flags)

    def sendall(self,buffer,flags=0):
        """
.. method:: sendall(buffer,flags=0)

        Send all data to the socket. The socket must be connected to a remote socket. 

        Unlike send(), this method continues to send data from bytes until either all data has been sent or an error occurs. 
        *None* is returned on success. On error, an exception is raised, and there is no way to determine how much data, if any, was successfully sent.
        """
        self.netdrv.sendall(self.channel,buffer,flags)

    def sendto(self,buffer,address,flags=0):
        """
.. method:: sendto(buffer,address,flags=0)

        Send data to the socket. The socket should not be connected to a remote socket, since the destination socket is specified by address.
        Return the number of bytes sent
        """
        #address = _address_to_address(address)
        return self.netdrv.sendto(self.channel,buffer,address,flags)

    def settimeout(self,timeout):
        """
.. method:: sendtimeout(timeout)
        
        Set a timeout on blocking socket operations. The *timeout* argument can be a nonnegative integer number expressing milliseconds, or *None*. 
        If a non-zero value is given, subsequent socket operations will raise a timeout exception if the timeout period value has elapsed before the operation has completed. 
        If zero is given, the socket is put in non-blocking mode. 
        If None is given, the socket is put in blocking mode.        
        """
        self.netdrv.setsockopt(self.channel,SOL_SOCKET,SO_RCVTIMEO,timeout)
        self.timeout = timeout

    def setsockopt(self,level,optname,value):
        return self.netdrv.setsockopt(self.channel,level,optname,value)

    def bind(self,address):
        """
.. method:: bind(address)
    
        Binds the socket to *address*. *address* can be:

            * an integer representing a port number. In this case ip is set to the local one
            * an ip address with a port 

        A tcp socket needs binding when it is used to accept incoming connection (e.g. a http server socket).
        A udp socket needs to be bound before any input/output operation. After binding, the udp socket will receive
        every packet incoming to *address*.
        """
        if type(address)==PSMALLINT:
            address = ("0.0.0.0",address)
        #address = _address_to_address(address)
        self.netdrv.bind(self.channel,address)

    def listen(self,maxlog=2):
        """
.. method:: listen(maxlog=2)
    
        Enables listening on the underlying tcp socket. A tcp socket in listening state can be used as a server socket to accept incoming connection.
        *maxlog* specifies the maximum number of waiting connections.
        """
        self.netdrv.listen(self.channel,maxlog)

    def accept(self):
        """
.. method:: accept()
        
        Blocks until an incoming connection is made on the underlying tcp socket.

        Returns a tuple (*sock*, *address*) where *sock* is a socket stream that can be used to communicate with the client and
        *address* is the client address.

        Here is an example of tcp server socket::

            # import the socket module
            import socket
            # import a module to access a net driver (wifi, eth,...)
            from wireless import wifi
            # import the actual net driver
            from driver.wifi.your_preferred_net_driver import your_preferred_net_driver

            # init the driver
            your_preferred_net_driver.init()

            # link the wifi to an AP
            wifi.link("Your Wifi SSID",WIFI_WPA2,"Your Wifi Password")

            # create a tcp socket
            sock = socket.socket(type=SOCK_STREAM)

            # bind the socket to port 80
            sock.bind(80)

            # set the socket in listening mode
            sock.listen()

            while True:
                # accept incoming connections from clients
                client,addr = sock.accept()
                # send something to the client and close
                client.sendall("Hello!")
                client.close()

        """
        sock,address = self.netdrv.accept(self.channel)
        return (socket(type=SOCK_STREAM,fileno=sock),address)



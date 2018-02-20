"""
.. _stdlib_eth:

.. module:: eth

********
Ethernet
********

This module implements a generic ethernet interface.
To function correctly it needs an ethernet driver to be loaded, so that the module can use
the driver to access the underlying hardware.

The link between the eth module and the ethernet driver is established without the programmer
intervention by the driver itself.


    """



def gethostbyname(hostname):
    """
.. function:: gethostbyname(hostname)        

        Translate a host name to IPv4 address format. The IPv4 address is returned as a string, such as "192.168.0.5". 
                
    """
    return __default_net["eth"].gethostbyname(hostname)


def select(rlist,wlist,xlist,timeout=None):
    """
.. function:: select(rlist, wlist, xlist, timeout=None)

   This is equivalent to the Unix *select* system call.
   The first three arguments are sequences of socket instances.

   * *rlist*: wait until ready for reading
   * *wlist*: wait until ready for writing
   * *xlist*: wait for an "exceptional condition" (not supported by every wifi driver)

   Empty sequences are allowed. The optional *timeout* argument specifies a time-out as an integer number
   in milliseconds.  When the *timeout* argument is omitted the function blocks until
   at least one socket is ready.  A *timeout* value of zero specifies a
   poll and never blocks.

   The return value is a triple of lists of objects that are ready: subsets of the
   first three arguments.  When the time-out is reached without a socket
   becoming ready, three empty lists are returned.
       
    """
    rrlist = [x.fileno() for x in rlist]
    wwlist = [y.fileno() for y in wlist]
    xxlist = [z.fileno() for z in xlist]
    rl,wl,xl = __default_net["eth"].select(rrlist,wwlist,xxlist,timeout)
    rr = [x for x in rlist if x.fileno() in rl]
    ww = [y for y in wlist if y.fileno() in wl]
    xx = [z for z in xlist if z.fileno() in xl]
    return (rr,ww,xx)


def link():
    """
.. function:: link()

        Activate the Ethernet PHY and try to establish a link.

        An exception can be raised if the link is not successful.

    """
    __default_net["eth"].link()

def unlink():
    """
.. function:: unlink()        

        Disable the Ethernet PHY and disconnect from the currently linked network.

    """ 
    __default_net["eth"].unlink()

def is_linked():
    """
.. function:: is_linked()        

        Return True if linked

    """ 
    return __default_net["eth"].is_linked()


def set_link_info(ip,mask,gw,dns):
    """
.. function:: set_link_info(ip,mask,gw,dns)        

        Set desired eth interface parameters:

            * ip, the static ipv4 address
            * mask, the network mask
            * gw, the default gateway
            * dns, the default dns

        If 0.0.0.0 is given, a default address will be used.

    """        
    __default_net["eth"].set_link_info(ip,mask,gw,dns)

def link_info():
    """
.. function:: link_info()        

        Return information on the currently established link.

        The result is a tuple where the elements are, in order:

            * The assigned IP as a string
            * The network mask as a string
            * The gateway IP as a string
            * The DNS IP as a string
            * The MAC address of the eth interface as a sequence of 6 bytes

    """        
    return __default_net["eth"].link_info()


"""
.. module:: NTPClient

******************
NTPClient Library
******************
    This library retrieve the current time from an NTP server.
    A method to convert the timestamp from ntc to a human readable format is available in the examples.
    
    """

import socket

class NTPClient():
    """
===============
NTPClient class
===============

.. class:: NTPClient(conn_ifc[, server = "0.pool.ntp.org"])
    
    Create an instance of the NTPClient class which allow communication with the NTPServer.

    If no server is provided, a default one will be used.
    
    :param conn_ifc: a connection interface that must be initialized by the user.
    :param server: the server to be used.
    """
    def __init__(self, conn_ifc = None, server = "0.pool.ntp.org"):
        self._conn = conn_ifc if conn_ifc is not None else __builtins__.__default_net["sock"][0]
        self._server = server

    def get_time(self,unix_timestamp=False):
        """
        .. method:: get_time()

            Retrieve the amount of second passed since January 1st 1900 from the NTP Server.

            Returns:
                The amount of second passed since January 1st 1900.

        """
        sock = socket.socket(type=socket.SOCK_DGRAM)
        sock.settimeout(1000)
        for i in range(0, 10):
            try:
                pkt = bytearray([0] * 48)
                pkt[0] = 0x1B
                
                ip_string = self._conn.gethostbyname(self._server)
                ip = socket.ip_to_tuple(ip_string)

                addr = (ip[0], ip[1], ip[2], ip[3], 123)
                sock.sendto(pkt, addr)

                res = sock.recv(48)
                ts = (res[40] << 24) | (res[41] << 16) | (res[42] << 8) | res[43]

                sock.close()
                if unix_timestamp:
                    return ts - (70*365 + 17)*86400
                return ts
     
            except Exception as e:
                sleep(100)
        else:
            sock.close()

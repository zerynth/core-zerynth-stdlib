"""
.. module:: streams

*******
Streams
*******

This module contains class definitions for streams.
A stream is an abstraction representing a channel to write and read sequences of bytes.
A stream is connected to something, being it a serial port or a socket or internal memory, from which it receives and to which it sends bytes.
Every stream instance implements the following methods:

    * read: removes bytes from the stream
    * write: puts bytes in the stream
    * available: checks the presence of readable bytes in the stream

*streams* implements the following streams:

    * base class :class:`streams.stream`
    * :class:`streams.serial`
    * :class:`streams.SocketStream`
    * :class:`streams.FileStream`
    * :class:`streams.ResourceStream`
"""


__builtins__.__default_stream_provider = __module__

class stream():
    """
================
The stream class
================

.. class:: stream

       This is the base class for streams and all streams must derive from this class.       
    """

    def __init__(self):
        pass
    def _readbuf(self,buf,size=1,ofs=0):
        """
.. method:: _readbuf(buffer,size=1,ofs=0)

        Reads bytes from the stream to *buffer*. If the stream contains no readable bytes it blocks until some bytes arrives or the underlying connection is lost (e.g. the server closes the socket).
        It directly access the driver linked to the stream issuing a native read command.
        In most cases :meth:`._readbuf` is called internally by other stream methods and should not be called directly.

        *buffer* is a byte sequence used to store the bytes just read. *size* is the number of bytes to be read and *ofs* is the position of *buffer* to start writing incoming bytes.
        Bytes are stored in *buffer* up to *buffer* size or less. 

        Returns the number of bytes read or 0 when the underlying connection is lost.
        """
        return self.channel.__ctl__(DRV_CMD_READ,self.hidx,buf,size,ofs)
    def write(self,buf):
        """
.. method:: write(buffer)        

        Writes the content of buffer to the stream. 

        *buffer* must be a byte sequence (bytes, bytearray or string).

        Returns the number of bytes actually written to the stream (can be less than the elements in *buffer*)
        """
        return self.channel.__ctl__(DRV_CMD_WRITE,self.hidx,buf)
    def read(self,size=1):
        """
.. method:: read(size=1)        

        Reads at most *size* bytes from the stream and returns a bytearray with the actual bytes read.
        
        *read* blocks if no bytes are available in the stream. 

        If *read* returns an empty bytearray the underlying stream can be considered disconnected.        
        """ 
        buf = bytearray(size)
        n = self._readbuf(buf,size)
        if n<0:
            raise IOError
        else:
            __elements_set(buf,n)
            return buf

    def readline(self,sep="\n",buffer=None,size=0,ofs=0):
        """
.. method:: readline(sep="\\\\n",buffer=None,size=0,ofs=0)        

        Reads bytes from the stream until *sep* is encountered (an end-of-line byte ("\\\\n") is default).
        
        Returns the line as a bytearray with *sep* included. 

        If *buffer* is given (as a bytearray), *buffer* is used to store the line up to *size* bytes, starting at offset *ofs*.

        If *readline* returns an empty bytearray the underlying stream can be considered disconnected. 
        """
        if buffer is None:
            line = bytearray(16)
            lb = 16
            pos = 0
        else:
            line = buffer
            lb = size
            pos = ofs
        sep = ord(sep)
        while True:
            n = self._readbuf(line,1,pos)
            if n<0:
                raise IOError
            elif n==0:
                __elements_set(line,pos)
                return line
            elif line[pos]==sep:
                __elements_set(line,pos+1)
                return line
            pos+=n
            if pos==lb:
                if buffer is None:
                    line.extend(bytearray(16))
                    lb+=16
                else:
                    __elements_set(line,pos)
                    return line

PARITY_NONE = 0
PARITY_EVEN = 1

STOPBIT_1 = 0
STOPBIT_1_HALF = 1
STOPBIT_2 = 2

BITSIZE_8 = 0

class serial(stream):
    """
================
The serial class
================

.. class:: serial(drivername=SERIAL0,baud=115200,set_default=True)

        This class implements a stream that can be used to connect to a serial port.
        It inhertis all of its methods from :class:`stream`.

        Initialize the serial port driver identified by *drivername* and starts it up with a baud rate of *baud*.
        Also, if *set_default* is True, sets itself as the default stream used by :func:`__builtins__.print`.
        This means that the serial stream will be the system default one.

        This is the code needed to print something on the default serial port: ::

            # import the streams module
            import streams

            # create a serial stream linked to SERIAL0 port
            streams.serial()

            # SERIAL0 is automatically selected as the default system stream,
            # therefore print will output everything to it
            print("Hello World!")

    """
    def __init__(self,drvname=SERIAL0,baud=115200,stopbits=STOPBIT_1,parity=PARITY_NONE,bitsize=BITSIZE_8,set_default=True):
        stream.__init__(self)
        self.channel = __driver(drvname)
        self.hidx = drvname&0xff
        self.channel.__ctl__(DRV_CMD_INIT,self.hidx,baud,parity,stopbits,bitsize)
        if set_default:
            __builtins__.__default_stream = self

    def available(self):
        """
.. method:: available()        

        Returns the number of characters that can be read without blocking.
                
        """
        return self.channel.__ctl__(DRV_CMD_AVAILABLE,self.hidx)

    def close(self):
        """
.. method:: close()        

        Close the stream linked to the underlying serial port.
        
        """
        return self.channel.__ctl__(DRV_CMD_DONE,self.hidx)


class SocketStream(stream):
    """
======================
The SocketStream class
======================

.. class:: SocketStream(sock)

        This class implements a stream that has a socket as a source of data.
        It inhertis all of its methods from :class:`stream`.

    """
    def __init__(self,sock):
        stream.__init__(self)
        self.socket = sock

    def _readbuf(self,buf,size=1,ofs=0):
        return self.socket.recv_into(buf,size,0,ofs)

    def write(self,buf):
        """
.. method:: write(buffer)        

        Writes the content of buffer to the stream. 

        *buffer* must be a byte sequence (bytes, bytearray or string).

        Returns the number of bytes actually written to the stream (can be less than the elements in *buffer*)
        """
        self.socket.sendall(buf)
        return len(buf)

    def close(self):
        """
.. method:: close()        

        Close the underlying socket.
        
        """
        self.socket.close()


SEEK_SET = 0
SEEK_CUR = 1
SEEK_END = 2

class FileStream(stream):
    """
======================
The FileStream class
======================

.. class:: FileStream(name,mode="rb")

        This class implements a stream that has a file as a source of data.
        It inherits all of its methods from :class:`stream`.

        It is just a stub at the moment. It is used by :class:`ResourceStream` only.

    """
    def __init__(self,name,mode="rb"):
        stream.__init__(self)
        self.name = name
        self.curpos=0

    def seek(self,offset,whence=SEEK_SET):
        """
.. method:: seek(offset,whence=SEEK_SET)        

        Move the current position to *offset* bytes with respect to *whence*.

        *whence* can be:

            * SEEK_SET: start of file
            * SEEK_CUR: current position
            * SEEK_END: end of file
        
        """        
        if whence==SEEK_SET:
            self.curpos=offset
        elif whence==SEEK_CUR:
            self.curpos+=offset
        else:
            self.curpos=self.size-offset
        if self.curpos<0:
            self.curpos=0
        elif self.curpos>self.size:
            self.curpos=self.size

class ResourceStream(FileStream):
    """
======================
The ResourceStream class
======================

.. class:: ResourceStream(name)

        This class implements a stream that has a flash saved resource as a source of data.
        It inherits all of its methods from :class:`FileStream`.

    """
    def __init__(self,name):
        FileStream.__init__(self,name)
        self.size,self.addr = __resource(name)

    def __getitem__(self, key):
        if type(key)==PSLICE:
            if key[2]!=1:
                raise UnsupportedError
            return __read_flash(self.addr+key[0],key[1]-key[0])
        elif type(key)==PSMALLINT:
            buf = __read_flash(self.addr+key,1)
            return buf[0]
        raise IndexError

    def _readbuf(self,buf,size=1,ofs=0):
        if self.curpos>=self.size: #eof
            return 0
        size = min(self.size-self.curpos,size)
        bb = __read_flash(self.addr+self.curpos,size)
        buf[ofs:ofs+size]=bb
        self.curpos+=size
        return size

    def __len__(self):
        return self.size

    def size(self):
        return self.size

    def write(self,buf):
        raise UnsupportedError


"""
.. module:: i2c

***
I2C
***

This module loads the I2C interface.

The I2C protocol is a multimaster and multislave serial communication protocol that allows sending and receiving data between the microcontroller (MCU) and low-speed peripheral. 
I2C needs only two wires, one called SCL functioning as a clock and one called SDA where the actual message bits are transferred.
I2C is a master-slave protocol, therefore a single master takes control of the bus and interacts with peripherals by sending on the bus the address of the peripheral the master wants to communicate with.
It is also possible to have more than one master on the same bus by using an arbitration policy; however, at the moment Zerynth VM supports only a single master protocol version where the microcontroller is the master.
Different versions of the I2C protocol use different clock speed:

    * Low-speed: up to 100kHz with 7 bits of peripheral address
    * Fast-mode: up to 400kHz, up to 10 bits of peripheral address
    * High-speed: up to 3.4 MHz
    * Fast-mode plus: up to 1 MHz
    * Ultra Fast-mode: up to 5 MHz

Zerynth VM generally supports low-speed and fast-mode if the microcontroller implements those versions.

I2C peripherals are usually implemented as a serial memory, meaning that they expose a list of registers that can be read and/or written. Therefore, for the master to correctly interact with a peripheral, some data must be known:

    * peripheral address: 7-bit or 10-bit address, hardwired in the peripheral and reported in the datasheet
    * register address: the peripheral memory location to be accessed

The master can perform three actions on the bus:

    * read: to access the value of a peripheral register. First, the master gets control of the bus, then the peripheral address is sent. The peripheral sends back the content of a predetermined register. The master releases the bus.
    * write: to change the value of a peripheral register. First, the master gets control of the bus, the peripheral address is sent, followed by the register address and the data to be written. The master releases the bus.
    * write-read: to exchange data with the peripherals in an atomic call. First, the master gets control of the bus, then the peripheral address is sent, followed by the data to be written (usually the address of a peripheral register). The bus is not released until the peripheral finishes to send its answer.


The I2C protocol provides mechanisms to detect bus errors. Zerynth VM catches bus errors and raises exceptions.

    """



__define(_I2CDRIVER_START,0)
__define(_I2CDRIVER_STOP,1)
__define(_I2CDRIVER_READ,2)
__define(_I2CDRIVER_WRITE,3)
__define(_I2CDRIVER_WRITE_READ,4)
__define(_I2CDRIVER_LOCK,5)
__define(_I2CDRIVER_UNLOCK,6)
__define(_I2CDRIVER_SET_ADDR,7)


@native_c("_vbl_i2c_init",["csrc/vbl/vbl_i2c.c"],["VHAL_I2C"])
def _vbl_i2c_init():
    pass


_vbl_i2c_init();


class I2C():
    """
================
The I2C class
================

.. class:: I2C(drvname,addr,clock=100000)

        Creates an I2C instance using the MCU I2C circuitry *drvname* (one of I2C0, I2C1, ... check pinmap for details). The created instance is configured to communicate with the slave peripheral identified by *addr*. *clock* is configured by default in slow mode.

    """    
    def __init__(self, drvname, addr, clock=100000):
        self.clock = clock
        self.drv = __driver(drvname)
        self.drvid = drvname&0xff
        self.addr = addr

    def set_addr(self,addr):
        """
.. method:: set_addr(addr)        

        Changes the peripheral address to communicate with.
                
        """
        self.addr = addr
        self.drv.__ctl__(_I2CDRIVER_SET_ADDR,self.drvid,addr)

    def start(self):
        """
.. method:: start()        

        I2C is started. It is necessary to start the driver before any communication can commence to transfer the I2C configuration parameter to the low level driver. If the I2C bus is already configured with different settings by another active istance of the I2C class, an exception is raised. 
                
        """
        #print(self.drv,self.drvid)
        self.drv.__ctl__(_I2CDRIVER_START,self.drvid,self.addr,self.clock)

    def write_bytes(self, *args,timeout=-1):
        """
.. method:: write_bytes(*args, timeout=-1)        

        *args* is converted to bytes and sent.

        """
        self.write(bytes(args),timeout,self.addr)

    def write(self, data, timeout=-1):
        """
.. method:: write(data, timeout=-1)        

        *data* is written.

        """
        tt = type(data)
        if tt==PSMALLINT:
            data = bytes((data,))
        elif tt>=PSHORTS and tt<=PLIST:
            data = bytes(data)
        self.drv.__ctl__(_I2CDRIVER_WRITE,self.drvid,data,timeout,self.addr)

    def read(self, n,timeout=-1):
        """
.. method:: read(n,timeout=-1)

        Returns a sequence of *n* bytes.

        """
        return self.drv.__ctl__(_I2CDRIVER_READ,self.drvid,n,timeout,self.addr)

    def write_read(self, data, n, timeout=-1):
        """
.. method:: write_read(data,n,timeout=-1)

        Writes *data* and then reads *n* bytes in a single call.

        """
        tt = type(data)
        if tt==PSMALLINT:
            data = bytes((data,))
        elif tt>=PSHORTS and tt<=PLIST:
            data = bytes(data)
        return self.drv.__ctl__(_I2CDRIVER_WRITE_READ,self.drvid,data,n,timeout,self.addr)


    def stop(self):
        """
.. method:: stop()        

        i2c is stopped and low level configuration disabled.

        """
        self.drv.__ctl__(_I2CDRIVER_STOP,self.drvid)

    def lock(self):
        """
.. method:: lock()        

        Locks the driver. It is useful when the same i2c object is used by multiple threads to avoid interferences.

        """
        self.drv.__ctl__(_I2CDRIVER_LOCK,self.drvid)

    def unlock(self):
        """
.. method:: unlock()        

        Unlocks the driver. It is useful when the same i2c object is used by multiple threads to avoid interferences.

        """
        self.drv.__ctl__(_I2CDRIVER_UNLOCK,self.drvid)



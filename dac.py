"""
.. module:: dac

***
DAC
***

This module loads the DAC (Digital Analog Converter) interface.
A digital to analog converter is a module to convert digital values into analog ones.

While builtin analogWrite *simulates* an analog value through the PWM peripheral, DAC output is a pure analog one.
This means higher accuracy, specially with high speed signals.

Example::

    import streams
    import adc # for analogRead

    import dac


    def readInput():
        while True:
            print("reading A1: ",analogRead(A1))
            sleep(500)

    streams.serial()
    pinMode(A1,INPUT)
    # read input in a separate thread
    thread(readInput)

    my_dac = dac.DAC(D8.DAC)
    my_dac.start()

    my_dac.write([100,200,900,800],1000,MILLIS,circular=True)

    """

__define(_DACDRIVER_START,0)
__define(_DACDRIVER_STOP,1)
__define(_DACDRIVER_WRITE,2)
__define(_DACDRIVER_LOCK,3)
__define(_DACDRIVER_UNLOCK,4)


@native_c("_vbl_dac_init",["csrc/vbl/vbl_dac.c"],["VHAL_DAC"])
def _vbl_dac_init():
    pass


_vbl_dac_init();
_dacdrv = __driver(DAC0)

class DAC():
    """
================
The DAC class
================

.. class:: DAC(pin)

        Creates a DAC instance on *pin* pin (look at board pinmap to see which pins are enabled for using the DAC peripheral).

    """
    def __init__(self, pin):
        self.drv = _dacdrv
        self.pin = pin
        self._buffer = None
        # self.drvid = drvname&0xff

    def start(self):
        """
.. method:: start()

        DAC is started. It is necessary to start the driver before writing any value.

        """
        #print(self.drv,self.drvid)
        self.drv.__ctl__(_DACDRIVER_START,self.pin)


    def write(self, data, timestep=1000, timeunit=MILLIS, circular=False):
        """
.. method:: write(data, timestep, timeunit, circular=False)

        *data* is output on selected pin.

        *data* parameter can be:

            * a single integer value::

                my_dac.write(666)

            * a list of values to be output every *timestep* *timeunit*econds until the end of the list is reached and last value is kept on output::

                my_dac.write([555,666,777],500,MILLIS)
                # one sample every 500 MILLISeconds

            * a list of values to be output every *timestep* *timeunit*econds, restarting from the beginning as soon as the end of the list is reached, until write method is called again::

                my_dac.write([555,666,777],500,MILLIS,circular=True)

        """
        tt = type(data)
        if tt==PSMALLINT:
            data = [data]
        self._buffer = shorts(data)
        self.drv.__ctl__(_DACDRIVER_WRITE,self._buffer,timestep,timeunit,circular,self.pin)


    def stop(self):
        """
.. method:: stop()

        dac is stopped and low level configuration disabled.

        """
        self.drv.__ctl__(_DACDRIVER_STOP,self.pin)

    def lock(self):
        """
.. method:: lock()

        Locks the driver. It is useful when the same dac object is used by multiple threads to avoid interferences.

        """
        self.drv.__ctl__(_DACDRIVER_LOCK,self.pin)

    def unlock(self):
        """
.. method:: unlock()

        Unlocks the driver. It is useful when the same dac object is used by multiple threads to avoid interferences.

        """
        self.drv.__ctl__(_DACDRIVER_UNLOCK,self.pin)

def remap(values,lowflex=660,highflex=3415,voltage=3.3):
    """
.. function:: remap(values,lowflex=660,highflex=3415)

    Some DAC chips have a limited voltage range (i.e. Sam3X by Atmel maps from ~3.3/6 V to ~3.3*(5/6) V) and remap given output values according to their operative ranges.
    This helper function allows the user to obtain the voltage value he would expect from a proportional DAC.

    Given a *value* in *values* list, DAC output will respect the following formula::

        output_voltage = board_voltage * (value/4096)

    Each *value* in the list must be an integer between *lowflex* and *highflex* in order to avoid a TypeError exception.

    *lowflex* and *highflex* default values are set to the correct values for Sam3X mcu.
    """
    mn = lowflex  # 0
    mx = highflex # 4095
    sl = (mx-mn)//4095
    # y = sl*x+mn
    res = []
    for val in values:
        if val < mn or val > mx:
            raise TypeError
        res.append((val-mn)//sl)
    return res

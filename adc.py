"""
.. module:: adc

****************************
Analog to Digital Conversion
****************************

This module loads the Analog to Digital Converter (adc) driver of the embedded device.

When imported, automatically sets the system adc driver to  the default one.


    """


_adc_drv = 0

@native_c("_vbl_adc_init",["csrc/vbl/vbl_adc.c"],["VHAL_ADC"])
def _vbl_adc_init():
    pass


def init(drvname,samples_per_second=800000):
    """
.. function:: init(drvname, samples_per_second=800000)

    Loads the adc driver identified by *drvname* and sets it up to read *samples_per_second* samples per second. The default is a sampling frequency of 0.8 MHz,
    valid values are dependent on the board.

    Returns the previous driver without disabling it.

    """
    global _adc_drv
    res = _adc_drv
    _vbl_adc_init()
    _adc_drv = __driver(drvname)
    _adc_drv.__ctl__(DRV_CMD_INIT,drvname&0xff,samples_per_second)
    __builtins__.__default_adc = _adc_drv
    return res


init(ADC0)

def done(drvname):
    """
.. function:: done(drvname)

    Unloads the adc driver identified by *drvname*.

    """
    global _adc_drv
    _adc_drv.__ctl__(DRV_CMD_DONE,drvname&0xff)
    __builtins__.__default_adc = None



def read(pin,samples=1):
    """
.. function:: read(pin, samples=1)

    Reads analog values from *pin* that must be one of the Ax pins. If *samples* is 1 or not given, returns the integer value read from *pin*.
    If *samples* is greater than 1, returns a tuple of integers of size *samples*.
    The maximum value returned by analogRead depends on the analog resolution of the board.

    *read* also accepts lists or tuples of pins and returns the corresponding tuple of tuples of samples: ::

        import adc

        x = adc.read([A4,A3,A5],6)

    this piece of code sets *x* to ((...),(...),(...)) where each inner tuple contains 6 samples taken from the corresponding channel.
    To use less memory, the inner tuples can be :class:`bytes`, or :class:`shorts` or normal tuples, depending on the hardware resolution of the adc unit.
    The number of sequentials pins that can be read in a single call depends on the specific board.

    """
    if not _adc_drv:
        return None
    return _adc_drv.__ctl__(DRV_CMD_READ,0,pin,samples)

 

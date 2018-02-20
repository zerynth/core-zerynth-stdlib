"""
.. module:: gc

*****************
Garbage Collector
*****************


This module allows the interaction with the garbage collector from Zerynth programs.


    """

@native_c("__gc",["csrc/gc/*"])
def __gc(code,timeout=0):
    pass

def info():
    """
.. function:: info()

    Returns a tuple of integers:

        0. Total memory in bytes
        1. Free memory in bytes
        2. Memory fragmentation percentage
        3. Number of allocated blocks
        4. Number of free blocks
        5. GC Period: milliseconds between forced collections
        6. Milliseconds since last collection
    """
    return __gc(GC_CMD_INFO)

def collect():
    """
.. function:: collect()

    Starts a collection
    """
    __gc(GC_CMD_COLLECT)

def enable(period=500):
    """
.. function:: enable(period=500)

    Activates the garbage collector with a GC Period of *period* milliseconds
    """
    __gc(GC_CMD_ENABLE,period)

def disable():
    """
.. function:: disable()

    Disable garbage collector
    """
    __gc(GC_CMD_DISABLE)
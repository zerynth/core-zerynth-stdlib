"""
.. module:: vm

**
VM
**

This module enables the inspection of the running VM.


    """

def info():
    """
.. function:: info()

    Return info about the running Virtual Machine. The result is a tuple with 9 elements:

    * Virtual Machine unique identifier (string)
    * Target device (string)
    * Virtual Machine version (string)
    * The RAM address where C memory starts
    * The Unix timestamp at which the bytecode was compiled
    * A 32bit integer customizable at compilation time (not yet supported, set at zero)
    * The bytecode version
    * The bytecode options (not yet supported, set at 0)
    * The bytecode size
    
    """
    return __vminfo()


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

    Return info about the running Virtual Machine. The result is a tuple with 3 elements (as strings):

    * Virtual Machine unique identifier
    * platform
    * version
    
    """
    return __vminfo()


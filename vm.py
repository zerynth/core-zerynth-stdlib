"""
.. module:: vm

**
VM
**

This module enables inspecting the running VM and controlling some runtime options.


    """

VM_INFO = 0
VM_SETOPT = 1
VM_GETOPT = 2
VM_ENCRYPT = 3
VM_DECRYPT = 4
VM_TRACEMASK = 5
VM_TRACE = 6

VM_OPT_RESET_ON_EXCEPTION = 0
VM_OPT_TRACE_ON_EXCEPTION = 1
VM_OPT_RESET_ON_HARDFAULT = 2
VM_OPT_TRACE_ON_HARDFAULT = 3
VM_OPT_RESET_ON_OUTOFMEMORY = 4
VM_OPT_TRACE_ON_OUTOFMEMORY = 5

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
    return __vmctrl(VM_INFO,0,0,0)


def set_option(opt,value):
    """
.. function:: set_option(opt,value)

    Set the VM Option :samp:`opt` to :samp:`value`. The following constants are avaiable for :samp:`opt`:

    * :samp:`VM_OPT_RESET_ON_EXCEPTION` : if :samp:`value` is 0, uncaught Python exception do not cause a microcontroller reset
    * :samp:`VM_OPT_TRACE_ON_EXCEPTION` : if :samp:`value` is 0, uncaught Python exception are not printed to console
    * :samp:`VM_OPT_RESET_ON_HARDFAULT` : if :samp:`value` is 0, microcontroller Hard Fault do not cause a microcontroller reset
    * :samp:`VM_OPT_TRACE_ON_HARDFAULT` : if :samp:`value` is 0, microcontroller Hard Fault dumps are not printed to console

    The functionalities for Hard Fault are not consistent across different microcontrollers due to different behaviours/capabilities of the underlying SDK.
    """
    return __vmctrl(VM_SETOPT,opt,value,0)

def get_option(opt):
    """
.. function:: get_option(opt)

    Retrieve the value of :samp:`opt`. Refer to :func:`set_option` for a description of :samp:`opt`.

    """
    return __vmctrl(VM_GETOPT,opt,0,0)

def encrypt(bin,nonce=0):
    """
.. function:: encrypt(bin,nonce=0)

    Return an encrypted copy of :samp:`bin`. The encrypted data is secured with a key which is unique per device and can only be
    decrypted by the same device that performed the encryption. To increase the key space, an integer :samp:`nonce` can be given
    that must be passed also for decryption.

    Raise :samp:`ValueError` if len(bin)<8

    """
    buf = bytes(bin)
    __vmctrl(VM_ENCRYPT,0,nonce,buf)
    return buf

def decrypt(bin,nonce=0):
    """
.. function:: decrypt(bin,nonce=0)

    Return a decrypted copy of :samp:`bin`. To increase the key space, an integer :samp:`nonce` can be given
    that must be the same one given for encryption.

    Raise :samp:`ValueError` if len(bin)<8

    """
    buf = bytes(bin)
    __vmctrl(VM_DECRYPT,0,nonce,buf)
    return buf

def set_tracemask(mask):
    return __vmctrl(VM_TRACEMASK,mask,0,0)

def trace(msg):
    return __vmctrl(VM_TRACE,0,0,msg)

def tracebin(msg):
    return __vmctrl(VM_TRACE,1,0,msg)


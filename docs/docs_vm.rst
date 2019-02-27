.. module:: vm

**
VM
**

This module enables the inspection of the running VM.


    
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
    
    
.. function:: set_option(opt,value)

    Set the VM Option :samp:`opt` to :samp:`value`. The following constants are avaiable for :samp:`opt`:

    * :samp:`VM_OPT_RESET_ON_EXCEPTION` : if :samp:`value` is 0, uncaught Python exception do not cause a microcontroller reset
    * :samp:`VM_OPT_TRACE_ON_EXCEPTION` : if :samp:`value` is 0, uncaught Python exception are not printed to console
    * :samp:`VM_OPT_RESET_ON_HARDFAULT` : if :samp:`value` is 0, microcontroller Hard Fault do not cause a microcontroller reset
    * :samp:`VM_OPT_TRACE_ON_HARDFAULT` : if :samp:`value` is 0, microcontroller Hard Fault dumps are not printed to console

    The functionalities for Hard Fault are not consistent across different microcontrollers due to different behaviours/capabilities of the underlying SDK.
    
.. function:: get_option(opt)

    Retrieve the value of :samp:`opt`. Refer to :func:`set_option` for a description of :samp:`opt`.

    

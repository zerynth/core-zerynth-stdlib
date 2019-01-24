.. module:: fota

.. _stdlib.fota:

***********************************
Firmware Over the Air update (FOTA)
***********************************

This module enables access to VM functionalities for updating firmware and/or VM at runtime.
It can be safely imported in every program, however its functions will raise `UnsupportedError` if the target VM is not enabled
for FOTA features (**available for Pro users only**).

In Zerynth FOTA can be performed for bytecode only or for bytecode and VM if the target device supports a multiple VM layout. 
In order to support FOTA updates, the flash memory of the target device is divided in segments that can hold either a VM or bytecode.
Moreover a small segment of persistent memory (not necessary flash) must be allocated to store the current and desired state of the firmware (FOTA record).

The following layouts show different flash organization for FOTA support: ::

      Support for FOTA update of bytecode only
    |------------------------------------------------------------------------------|
    |                  |                           |                           |   |
    |       VM         |      Bytecode Slot 0      |      Bytecode Slot 1      |   | <-- FOTA Record
    |                  |                           |                           |   |
    |------------------------------------------------------------------------------|
    
      Support for FOTA update of bytecode and VM
    |------------------------------------------------------------------------------|
    |              |                      |             |                      |   |
    |    VM 0      |    Bytecode Slot 0   |   VM 1      |    Bytecode Slot 1   |   | <-- FOTA Record
    |              |                      |             |                      |   |
    |------------------------------------------------------------------------------|


FOTA process for bytecode only
------------------------------

The FOTA process consists of the following steps:

    * check the FOTA record structure to ensure that the current VM+Bytecode slot configuration is functioning correctly (:func:`get_record`)
    * identify the available slot for the new bytecode (:func:`find_bytecode_slot`)
    * erase the bytecode slot to make room for new bytecode (:func:`erase_slot`)
    * write the binary representation of the new bytecode to the slot (:func:`write_slot`)
    * optionally check that the new bytecode has been written correctly (:func:`checksum_slot`)
    * modify the FOTA record with the new desired configuration (:func:`attempt`)
    * restart the VM in the new configuration (:func:`restart`)
    * the new bytecode should check that everything is ok and confirm the configuration (:func:`accept`). In case the new bytecode is faulty, a simple restart will bring the system back in the previously working configuration.
    


FOTA process for bytecode and VM
--------------------------------

For some target devices it is possible to change both the bytecode and the VM in a single FOTA process.

The FOTA process in this case consists of the following steps:

    * check the FOTA record structure to ensure that the current VM+Bytecode slot configuration is functioning correctly (:func:`get_record`)
    * identify the available slot for the new bytecode (:func:`find_bytecode_slot`) and for the new VM (:func:`find_vm_slot`)
    * erase the bytecode slot and vm slot to make room for new firmware (:func:`erase_slot`)
    * write the binary representation of the new bytecode and vm to the slots (:func:`write_slot`)
    * optionally check that the new bytecode and vm have been written correctly (:func:`checksum_slot`)
    * modify the FOTA record with the new desired configuration (:func:`attempt`)
    * restart the VM in the new configuration (:func:`restart`)
    * the previous VM will start and behave as a bootloader passing control to the new VM
    * the new bytecode should check that everything is ok and confirm the configuration (:func:`accept`). In case the new bytecode is faulty, a simple restart will bring the system back in the previously working configuration.


.. important:: for the new firmware to work after the FOTA process, it must be compiled and linked for the correct slots using the dedicated ZTC command (:ref:`link <ztc-cmd-link>`)

.. important:: manually performing the FOTA process can be difficult. FOTA features can be easily tested by using the :ref:`Zerynth ADM Library <lib.zerynth.zadm>`.

.. important:: The FOTA process can be performed WITHOUT stopping the current firmware execution until the very end of the process

FOTA functions
--------------

.. function:: get_record()

    Return the FOTA record (fota) as a tuple of integers following this scheme:

        * fota[0] is 0 if the FOTA record is not valid
        * fota[1] is the index of the FOTA VM
        * fota[2] is the slot of the current VM (as an index starting from 0)
        * fota[3] is the slot of the last working VM (as an index starting from 0)
        * fota[4] is the slot of the current bytecode (as an index starting from 0)
        * fota[5] is the slot of the last working bytecode (as an index starting from 0)
        * fota[6] is the flash address of the current bytecode
        * fota[7] is the flash address of the current VM
        * fota[8] is the OTA chunk

.. function:: set_record(newfota)
    
    Set the FOTA record (fota) to the values in newfota according to this scheme:

        * fota[2] = newfota[0], sets the current VM for next restart
        * fota[3] = newfota[1], sets the last working VM
        * fota[4] = newfota[2], set the current bytecode for the next restart
        * fota[5] = newfota[3], set the last working bytecode

        All other fota elements are managed by the VM and cannot be changed

.. function:: find_bytecode_slot()

    Return the address of next available bytecode slot.


.. function:: find_vm_slot()

    Return the address of the next available vm slot. If the result of the function is equal to fota[7], the current VM does not support VM updates.

.. function:: erase_slot(addr, size)

    Erase the slot (either bytecode or VM) starting at :samp:`addr` for at least :samp:`size` bytes. Since flash memories are often organized in sectors, the erased size can be larger than the requested size.

.. function:: write_slot(addr, block)

    Write :samp:`block` (bytes, bytearray or string) at :samp:`addr` where :samp:`addr` is an address contained in a bytecode or VM slot. This function does not keep count of the written blocks, it is up to the programmer to update the address correctly.

.. function:: checksum_slot(addr, size)

    Return the MD5 checksum (as a bytes object) of the slot starting at :samp:`addr` and extending for :samp:`size` bytes.

.. function:: restart()

    Reset the microcontroller and restart the system

.. function:: accept()
    
    Modify the FOTA record to make the current configuration the last working configuration. Must be called by the new bytecode in order to end the FOTA process correctly.

.. function:: attempt(bcslot, vmslot)

    Modify the FOTA record to make :samp:`bcslot` and :samp:`vmslot` (expressed as indexes starting from 0), the test configuration to be tried on restart.

    

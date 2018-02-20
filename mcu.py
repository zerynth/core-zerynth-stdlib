"""
.. module:: mcu

***
MCU
***

This module enables the access to low level microcontroller functionalities like:

	* unique identifiers
	* soft reset

    """

@c_native("_mcu_reset",["csrc/mcu/mcu.c"])
def reset():
	"""
.. function:: reset()

	Restart the microcontroller.
	
.. function:: uid()

	Return a bytes object containing the unique id of the mcu

	"""
	pass

uid = __mcuuid

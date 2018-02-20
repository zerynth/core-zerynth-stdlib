"""
.. module:: sfw

***************
Secure Firmware
***************

This module enables access to secure firmware functionalities specific to the target microcontroller.
It can be safely imported in every program, however its functions will raise `UnsupportedError` if the target VM is not enabled
for secure firmware features (**available for Pro users only**).

There are many ways of securing the firmware running on a microcontroller:

    * disable access to the flash memory wher the firmware resides with external means (e.g. JTAG probes)
    * reserve some memory areas to critical parts of the system (e.g. VM running in mcu Trust Zones)
    * detect device tampering and take action accordingly (e.g. erase firmware)
    * detect and recover from firmware malfunctions (e.g. watchdogs)

Of the above features, only watchdogs are implemented in all architectures; the rest of features are strongly platform dependent.
This module allow access to the microcontroller watchdogs and will enable access to anti-tampering features soon. 

Watchdogs
---------

A Watchdog is usually implemented as a countdown timer that resets the microcontroller if it is not refreshed by firmware in a specified timeout period.
The action of refreshing the watchdog timer is often called "kicking" or "feeding" the watchdog. A windowed watchdog is a specially configured watchdog that resets the microcontroller
both on a normal timeout or on a kick that happens before a specified time from last refresh.

Visually: ::

    Normal Watchdog time window
    |----------------------------------------------|  --> a reset happens!
    0                                              timeout 
         A kick here restarts the watchdog

    Windowed Watchodog
    |............|---------------------------------|  --> a reset happens!
    0            t1                                timeout
      A kick here           A kick here
      reset the mcu!        restarts the watchdog

Each microcontroller family has its own watchdog peculiarities, described in the following sections:

    * :ref:`STM32F families <sfw-stm32f>`
    * :ref:`NXP K64 families <sfw-nxpk64>`
    * :ref:`Microchip SAMD21 <sfw-samd21>`


This module provides the following functions for watchdog usage:


.. function:: watchdog(time0, timeout)

    Enable the watchdog. If :samp:`time0` is zero, the watchdog is configured in normal mode; if it is greater than zero, the watchdog is configured in windowed mode (if supported) in such a way that a kick in the first :samp:`time0` milliseconds resets the device. 
    :samp:`timeout` configures the number of milliseconds the whole watchdog window lasts. Once started, the watchdog CAN'T be stopped!

.. function:: kick()

    Refresh the watchdog, resetting its time window.

.. function:: watchdog_triggered()

    Return ``True`` if the microcontroller has been reset by the watchdog, ``False`` otherwise.



.. _sfw-stm32f:
Watchdogs for STM32Fxx families
-------------------------------

For STM32 microcontrollers the watchdog is implemented using the IWDG. The maximum timeout is 32768 milliseconds and there is no support for windowed mode (an exception is raised if :samp:`time0` is greater than zero in :func:`watchdog`).
The watchdog is disabled in low power modes.

.. _sfw-nxpk64:
Watchdogs for NXP K64 families
------------------------------

For K64 microcontrollers the watchdog is implemented using WDOG. The WDOG is clocked by the low power oscillator with a frequency of 1kHz.T The maximum timeout is around 74 hours and windowed mode is supported.
The watchdog is disabled in low power modes.



.. _sfw-samd21:
Watchdogs for Microchip SAMD21
------------------------------

For SAMD21 microcontrollers the watchdog is implemented using WDT. The WDT is clocked by the ultra low power oscillator with a frequency of 1kHz. In normal mode the maximum timeout is 16 seconds. In windowed mode :samp:`timeout` can reach 32 seconds and :samp:`time0` 16 seconds; this is because windowed mode works with two timers, the first for :samp:`time0` and the second for :samp:`timeout`.
The timeout granularity of WDT is quite coarse, allowing only 12 different timeout values, expressed in WDT clock cycles:

    * 8 cycles, 8 milliseconds
    * 16 cycles, 16 milliseconds

    ...
    
    * 16384 cycles, 16384 milliseconds

The :func:`watchdog` function will select the nearest allowed time rounding up with respect to the specified time.


    """





watchdog = __sfw_setup_wdog

kick = __sfw_kick_wdog

def watchdog_triggered():
    return __pwr_wakeup_reason()==3

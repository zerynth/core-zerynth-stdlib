.. module:: timers

***************
Software Timers
***************

This module contain the :class:`timer` to handle time and timed events


.. function:: now()

    Return the number of milliseconds since the start of the program.
.. class:: timer()

    Creates a new timer.
    
.. method:: one_shot(delay,fun,arg=None)
    
    Activates the timer in one shot mode. Function *fun(arg)* is executed only once after *delay* milliseconds.

        
.. method:: interval(period,fun,arg=None)
    
    Activates the timer in interval mode. Function *fun(arg)* is executed every *period* milliseconds.

        
.. method:: clear()
    
    Disable the timer.

        
.. method:: destroy()
    
    Disable the timer and kills it.

        
.. method:: start()
        
    Start the timer. A started timer begins counting the number of passing milliseconds. Such number can be read by calling
    :ref:`timer.get()`.
        
.. method:: reset()
    
    Reset the timer. A reset timer restarts counting the number of passing milliseconds from zero.

    Returns the number of milliseconds passed since the start or the last reset.
        
.. method:: get()
    
    Return the number of milliseconds passed since the start or the last reset.
        

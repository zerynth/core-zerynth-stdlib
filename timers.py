"""
.. module:: timers

***************
Software Timers
***************

This module contain the :class:`timer` to handle time and timed events


.. function:: now()

    Return the number of milliseconds since the start of the program.

"""

now = __timer_get

def recurring_timer(instance,period,fn,arg=None):
    fn(arg)
    if instance.tm:
        __timer_set(instance.tm,period,recurring_timer,instance,period,fn,arg)


class timer():
    """
.. class:: timer()

    Creates a new timer.
    """
    def __init__(self):
        self.tm = __new_timer()
        self.time = None
    
    def one_shot(self,delay,fn,arg=None):
        """
.. method:: one_shot(delay,fun,arg=None)
    
    Activates the timer in one shot mode. Function *fun(arg)* is executed only once after *delay* milliseconds.

        """
        __timer_set(self.tm,delay,fn,arg)
    def interval(self,period,fn,arg=None):
        """
.. method:: interval(period,fun,arg=None)
    
    Activates the timer in interval mode. Function *fun(arg)* is executed every *period* milliseconds.

        """
        __timer_set(self.tm,period,recurring_timer,self,period,fn,arg)

    def clear(self):
        """
.. method:: clear()
    
    Disable the timer.

        """
        __timer_set(self.tm,-1,None)

    def destroy(self):
        """
.. method:: destroy()
    
    Disable the timer and kills it.

        """
        __timer_set(self.tm,-1,None)
        self.tm=None

    def start(self):
        """
.. method:: start()
        
    Start the timer. A started timer begins counting the number of passing milliseconds. Such number can be read by calling
    :ref:`timer.get()`.
        """
        self.time = __timer_get()
    
    def reset(self):
        """
.. method:: reset()
    
    Reset the timer. A reset timer restarts counting the number of passing milliseconds from zero.

    Returns the number of milliseconds passed since the start or the last reset.
        """
        now = __timer_get()
        ret = self.get()
        if now>=self.time:
            ret = now-self.time
        else:
            ret = 0x3fffffff-self.time+now
        self.time = now
        return ret

    def get(self):
        """
.. method:: get()
    
    Return the number of milliseconds passed since the start or the last reset.
        """
        now = __timer_get()
        if now>=self.time:
            ret = now-self.time
        else:
            ret = 0x3fffffff-self.time+now
        return ret
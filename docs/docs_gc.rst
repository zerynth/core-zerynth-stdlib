.. module:: gc

*****************
Garbage Collector
*****************


This module allows the interaction with the garbage collector from Zerynth programs.


    
.. function:: info()

    Returns a tuple of integers:

        0. Total memory in bytes
        1. Free memory in bytes
        2. Memory fragmentation percentage
        3. Number of allocated blocks
        4. Number of free blocks
        5. GC Period: milliseconds between forced collections
        6. Milliseconds since last collection
    
.. function:: collect()

    Starts a collection
    
.. function:: enable(period=500)

    Activates the garbage collector with a GC Period of *period* milliseconds
    
.. function:: disable()

    Disable garbage collector
    

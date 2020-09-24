"""
.. module:: queue

*****
Queue
*****


The queue module implements multi-producer, multi-consumer queues. It is especially useful in threaded programming when information must be exchanged safely between multiple threads. The Queue class in this module implements all the required locking semantics. 

   """

import threading

new_exception(QueueFull,Exception,"Queue is full")
new_exception(QueueEmpty,Exception,"Queue is empty")

class Queue():
    """
==========
Queue class
==========

.. class:: Queue(maxsize=0)

    Constructor for a FIFO queue.  *maxsize* is an integer that sets the upperbound
    limit on the number of items that can be placed in the queue.  Insertion will
    block once this size has been reached, until queue items are consumed.  If
    *maxsize* is less than or equal to zero, the queue size is infinite.

    """
    def __init__(self,maxsize=0):
        self.maxsize=maxsize
        self.mutex = threading.Lock()
        self.not_empty = threading.Condition(self.mutex)
        self.not_full = threading.Condition(self.mutex)
        self.q=[]
    
    def qsize(self):
        """
.. method:: qsize()

    Return the approximate size of the queue.  Note, qsize() > 0 doesn't
    guarantee that a subsequent get() will not block, nor will qsize() < maxsize
    guarantee that put() will not block.

        """
        self.mutex.acquire()
        res = len(self.q)
        self.mutex.release()
        return res

    def full(self):
        """
.. method:: full()

    Return ``True`` if the queue is full, ``False`` otherwise.  If full()
    returns ``True`` it doesn't guarantee that a subsequent call to get()
    will not block.  Similarly, if full() returns ``False`` it doesn't
    guarantee that a subsequent call to put() will not block.        
        """
        self.mutex.acquire()
        res = self.maxsize > 0 and len(self.q)>=self.maxsize
        self.mutex.release()
        return res
    
    def empty(self):
        """
.. method:: empty()

    Return ``True`` if the queue is empty, ``False`` otherwise.  If empty()
    returns ``True`` it doesn't guarantee that a subsequent call to put()
    will not block.  Similarly, if empty() returns ``False`` it doesn't
    guarantee that a subsequent call to get() will not block.

        """
        self.mutex.acquire()
        res = len(self.q)==0
        self.mutex.release()
        return res
    
    def put(self,obj,block=True,timeout=-1):
        """
.. method:: put(obj,block=True,timeout=-1)

    Insert *obj* into the queue. If the queue is full, and *block* is True, block until a free slot becomes available. If *block* is False, raise QueueFull.
    If *timeout* is greater than zero, waits for the specified amount of milliseconds before raising QueueFull exception.

        """
        self.not_full.acquire()
        while self.maxsize > 0 and len(self.q)>=self.maxsize:
            if block:
                block = self.not_full.wait(timeout)
            if not block:
                self.not_full.release()
                raise QueueFull
        self.q.append(obj)
        self.not_empty.notify()
        self.not_full.release()
    
    def get(self,timeout=-1):
        """
.. method:: get(timeout=-1)

    :param timeout: set this argument different from -1 to make the Queue.get() unblocking (default timeout=-1 means get blocking)

    Remove and return an object out of the queue. If the queue is empty, block until an item is available or timeout occurred.

        """
        self.not_empty.acquire()
        while len(self.q)==0:
            block = self.not_empty.wait(timeout)
            if not block:
                self.not_empty.release()
                raise QueueEmpty
        res = self.q[0]
        self.q=self.q[1:]
        self.not_full.notify()
        self.not_empty.release()
        return res

    def peek(self):
        """
.. method:: peek()

    Return the object at the head of the queue without removing it. If the queue is empty, wait until an item is available.

        """
        self.not_empty.acquire()
        while len(self.q)==0:
            self.not_empty.wait()
        res = self.q[0]
        self.not_empty.release()
        return res

    def clear(self):
        """
.. method:: clear()

    Clear the queue by removing all elements. 

        """
        self.mutex.acquire()
        self.q = []
        self.not_full.notify()
        self.mutex.release()

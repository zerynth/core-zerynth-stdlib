"""
.. module:: fifo

****
Fifo
****

This module implements no thread-safe fifo queues.
A fifo queue behaves in such a way that the first element inserted in the queue is also the first element to be removed (first in, first out).

   """

new_exception(FifoFullError,Exception)
new_exception(FifoEmptyError,Exception)

class Fifo():
    """
==========
Fifo class
==========

.. class:: Fifo(size=16,only_bytes=False)

    Create a Fifo instance with *size* "places" for items. 
    If *only_bytes* is True, the Fifo will use a bytearray to store bytes; if False it will use a list.

    """
    def __init__(self,size=16,only_bytes=False):
        if not only_bytes:
            self._fifo = [None]*size
        else:
            self._fifo=bytearray(size)
        self.l = size
        self.head=0
        self.elem=0

    def is_full(self):
        """
.. method:: is_full()

    Return True if the fifo is full

        """
        return self.elem==self.l
    
    def is_empty(self):
        """
.. method:: is_empty()

    Return True if the fifo is empty

        """
        return self.elem==0
    
    def put(self,obj):
        """
.. method:: put(obj)

    Insert *obj* into the fifo queue. Raise *FifoFullError* if the fifo is full.

        """
        if self.elem==self.l:
            raise FifoFullError
        self._fifo[ (self.head+self.elem)%self.l]=obj
        self.elem+=1
    
    def get(self):
        """
.. method:: get()

    Get an object out of the fifo queue. Raise *FifoEmptyError* if the fifo is empty.

        """
        if self.elem==0:
            raise FifoEmptyError
        res = self._fifo[self.head % self.l]
        self.elem-=1
        self.head+=1
        return res

    def peek(self):
        """
.. method:: peek()

    Return the object at the head of the fifo queu without removing it. Raise *FifoEmptyError* if the fifo is empty.

        """
        if self.elem==0:
            raise FifoEmptyError
        return self._fifo[self.head % self.l]

    def put_all(self,objs):
        """
.. method:: put_all(objs)

    Put every item of *objs* into the fifo queue.

        """
        for obj in objs:
            self.put(obj)
            
    def elements(self):
        """
.. method:: elements()

    Return the number of items in the queue.

        """
        return self.elem

    def clear(self):
        """
.. method:: clear()

    Clear the fifo by removing all elements

        """
        self.head=0
        self.elem=0
        # also clear the list to be garbage collector friendly
        if type(self._fifo)==PLIST:
            self._fifo = [None]*self.l
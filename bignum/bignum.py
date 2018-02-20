"""
.. module:: bignum

******
Bignum
******

This module provides function and classes to operate on arbitrary precision integers.
It is based on the very perfomant library `TomMath <https://github.com/libtom/libtommath>`_ slightly
modified to be compatible with Zerynth memory manager.

    """

@c_native("_zbn_new",[
    "csrc/bn_mp_init.c",
    "csrc/bn_mp_init_set.c",
    "csrc/bn_mp_init_set_int.c",
    "csrc/bn_mp_set.c",
    "csrc/bn_mp_set_int.c",
    "csrc/bn_mp_lshd.c",
    "csrc/bn_mp_copy.c",
    "csrc/bn_mp_shrink.c",
    "csrc/bn_mp_grow.c",
    "csrc/bn_mp_clamp.c",
    "csrc/bn_mp_mul_2d.c",
    "csrc/bn_mp_zero.c",
    "csrc/bn_mp_add_d.c",
    "csrc/bn_mp_sub_d.c",
    "csrc/bn_mp_read_radix_n.c",
    "csrc/zbn_ifc.c",
    ],[],[])
def _bn_new(val):
    pass


@c_native("_zbn_tobase",[
    "csrc/bn_mp_clamp.c",
    "csrc/bn_mp_clear.c",
    "csrc/bn_mp_copy.c",
    "csrc/bn_mp_div_2d.c",
    "csrc/bn_mp_div_3.c",
    "csrc/bn_mp_div_d.c",
    "csrc/bn_mp_exch.c",
    "csrc/bn_mp_grow.c",
    "csrc/bn_mp_init_copy.c",
    "csrc/bn_mp_init_size.c",
    "csrc/bn_mp_mod_2d.c",
    "csrc/bn_mp_rshd.c",
    "csrc/bn_mp_toradix_n.c",
    "csrc/bn_mp_zero.c",
    "csrc/bn_reverse.c"
    ],[],[])
def _bn_tobase(num,base):
    pass

@c_native("_zbn_add",[
    "csrc/bn_mp_add.c",
    "csrc/bn_mp_clamp.c",
    "csrc/bn_mp_cmp_mag.c",
    "csrc/bn_mp_grow.c",
    "csrc/bn_s_mp_add.c",
    "csrc/bn_s_mp_sub.c"
    ],[],[])
def _bn_add(a,b):
    pass

@c_native("_zbn_sub",[
    "csrc/bn_mp_sub.c",
    "csrc/bn_mp_clamp.c",
    "csrc/bn_mp_cmp_mag.c",
    "csrc/bn_mp_grow.c",
    "csrc/bn_s_mp_add.c",
    "csrc/bn_s_mp_sub.c"
    ],[],[])
def _bn_sub(a,b):
    pass

@c_native("_zbn_mul",[
    "csrc/bn_mp_mul.c",
    "csrc/bn_mp_karatsuba_mul.c",
    "csrc/bn_s_mp_mul_digs.c",
    "csrc/bn_fast_s_mp_mul_digs.c",
    "csrc/bn_mp_toom_mul.c",
    "csrc/bn_mp_init_multi.c",
    "csrc/bn_mp_mul_d.c",
    "csrc/bn_mp_clear_multi.c",
    "csrc/bn_mp_div_2.c",
    "csrc/bn_mp_mul_2.c",
    "csrc/bn_mp_clamp.c",
    "csrc/bn_mp_cmp_mag.c",
    "csrc/bn_mp_grow.c",
    "csrc/bn_s_mp_add.c",
    "csrc/bn_s_mp_sub.c"
    ],[],[])
def _bn_mul(a,b):
    pass

@c_native("_zbn_div",[
    "csrc/bn_mp_div.c",
    "csrc/bn_s_mp_mul_digs.c",
    "csrc/bn_fast_s_mp_mul_digs.c",
    "csrc/bn_mp_count_bits.c",
    "csrc/bn_mp_init_multi.c",
    "csrc/bn_mp_mul_d.c",
    "csrc/bn_mp_clear_multi.c",
    "csrc/bn_mp_div_2.c",
    "csrc/bn_mp_mul_2.c",
    "csrc/bn_mp_clamp.c",
    "csrc/bn_mp_cmp.c",
    "csrc/bn_mp_cmp_mag.c",
    "csrc/bn_mp_grow.c",
    "csrc/bn_s_mp_add.c",
    "csrc/bn_s_mp_sub.c"
    ],[],[])
def _bn_div(a,b):
    pass


@c_native("_zbn_cmp",[
    "csrc/bn_mp_cmp.c",
    "csrc/bn_mp_cmp_mag.c",
    ],[],[])
def _bn_cmp(a,b):
    pass


class BigNum():
    """
================
The Bignum class
================

.. class:: BigNum(val=0)

       This class represents a big integer number with arbitrary precision. A big number instance can be initialized with
       a value *val*. *val* can be a standard integer or a string representing the number. The string is accepted if it is in base 16
       prefixed with  '0x' or in base 10. Signed number are accepted. At the moment, bytearray or bytes representation are not supported.

       BigNum instances are compatible with streams and, if printed, are automatically converted to the base 10 string format.

       BigNum instances are easy to use: ::

            from bignum import bignum as bg
            import streams

            streams.serial()

            big = bg.BigNum("1234567890987654321")
            one = bg.BigNum(1)

            while True:
                print(big)
                big.iadd(one)
                sleep(1000)




    """
    def __init__(self,val=0):
        if val is not None:
            self.num = _bn_new(val)

    def add(self,b):
        """
.. method:: add(b)

        Return a new big number instance equal to the addition of the current instance and *b*.
        """
        bg = BigNum(None)
        bg.num = _bn_add(self.num,b.num)
        return bg
    
    def iadd(self,b):
        """
.. method:: iadd(b)

        Add to the current instance the big number *b*. Return :samp:`None`
        """
        self.num = _bn_add(self.num,b.num)

    def sub(self,b):
        """
.. method:: sub(b)

        Return a new big number instance equal to the difference of the current instance and *b*.
        """
        bg = BigNum(None)
        bg.num = _bn_sub(self.num,b.num)
        return bg
    
    def isub(self,b):
        """
.. method:: isub(b)

        Subtracts to the current instance the big number *b*. Return :samp:`None`
        """
        self.num = _bn_sub(self.num,b.num)

    def mul(self,b):
        """
.. method:: mul(b)

        Return a new big number instance equal to the multiplication of the current instance and *b*.
        """
        bg = BigNum(None)
        bg.num = _bn_mul(self.num,b.num)
        return bg
    
    def imul(self,b):
        """
.. method:: imul(,b)

        Multiply the current instance for the big number *b*. Return :samp:`None`
        """
        self.num = _bn_mul(self.num,b.num)

    def div(self,b):
        """
.. method:: div(b)

        Return a new big number instance equal to the division of the current instance by *b*.
        """
        bg = BigNum(None)
        bg.num, r = _bn_div(self.num,b.num)
        return bg
    
    def idiv(self,b):
        """
.. method:: idiv(b)

        Divides the current instance for the big number *b*. Return :samp:`None`
        """
        self.num, r = _bn_div(self.num,b.num)
    
    def mod(self,b):
        """
.. method:: mod(b)

        Return a new big number instance equal to the remainder of the division of the current instance by *b*.
        """
        bg = BigNum(None)
        q, bg.num = _bn_div(self.num,b.num)
        return bg
    
    def imod(self,b):
        """
.. method:: imod(b)

        Set the current instance to the remainder of the division by *b*. Return :samp:`None`
        """
        q, self.num = _bn_div(self.num,b.num)

    def divmod(self,b):
        """
.. method:: divmod(b)

        Return a tuple (q,r) of new big number instances representing the quotient *q* and the remainder *r* of the division of the current instance by *b*.
        """
        bq = BigNum(None)
        br = BigNum(None)
        bq.num, br.num = _bn_div(self.num,b.num)
        return bq,br

#define MP_LT        -1   /* less than */
#define MP_EQ         0   /* equal to */
#define MP_GT         1   /* greater than */

#define MP_ZPOS       0   /* positive integer */
#define MP_NEG        1   /* negative */
    
    def eq(self,b):
        """
..method:: eq(b)

    Return True if the current instance is equal to the big number *b*, False otherwise.
        """
        ret = _bn_cmp(self.num,b.num)
        if ret==0:
            return True
        return False
   
    def lt(self,b):
        """
..method:: lt(b)

    Return True if the current instance is less than the big number *b*, False otherwise.
        """
        ret = _bn_cmp(self.num,b.num)
        if ret>=0:
            return False
        return True
    
    def gt(self,b):
        """
..method:: gt(b)

    Return True if the current instance is greater than the big number *b*, False otherwise.
        """
        ret = _bn_cmp(self.num,b.num)
        if ret<=0:
            return False
        return True

    def lte(self,b):
        """
..method:: lte(b)

    Return True if the current instance is less than or equal to the big number *b*, False otherwise.
        """
        ret = _bn_cmp(self.num,b.num)
        if ret>0:
            return False
        return True
    
    def gte(self,b):
        """
..method:: gte(b)

    Return True if the current instance is greater than or equal to the big number *b*, False otherwise.
        """
        ret = _bn_cmp(self.num,b.num)
        if ret<0:
            return False
        return True

    def sign(self):
        """
..method:: sign()

    Return 1 if the current instance is a positive number, -1 if the current instance is a negative number, 0 if it is equal to zero.
        """
        b = BigNum(0)
        if self.lt(b):
            return -1
        elif self.gt(b):
            return 1
        return 0


    def to_base(self,base):
        """
.. method:: to_base(base)

        Return a string representation of the big number in base *base*. Allowed values for *base* are in the range 2..64.
        """
        return _bn_tobase(self.num,base)

    def __str__(self):
        return self.to_base(10)




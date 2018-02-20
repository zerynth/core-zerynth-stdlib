"""
.. module:: math

****
Math
****


This module implements various mathematical functions. 

Except when explicitly noted otherwise, all return values are floats. The underlying implementations works on double precision floats that are converted back to single precision if the VM does not support double precision.

No exceptions are raised: in case of error, the return value can be infinite or NaN. Such cases can be checked with the provided functions.

The following constants are defined:

    * :samp:`pi` 3.14159265
    * :samp:`e`  2.71828182

    """

pi = 3.14159265
e =  2.71828182


##### TRIGONOMETRY

@native_c("__tan",["csrc/math/trig.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def tan(rad):
    """
.. function:: tan(x)

   Return the tangent of *x* radians.
    """
    pass

@native_c("__cos",["csrc/math/trig.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def cos(rad):
    """
.. function:: cos(x)

   Return the cosine of *x* radians.
    """
    pass

@native_c("__sin",["csrc/math/trig.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def sin(rad):
    """
.. function:: sin(x)

   Return the sine of *x* radians.
    """
    pass

@native_c("__atan2",["csrc/math/atan2.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def atan2(y,x):
    """
.. function:: atan2(y, x)

   Return ``atan(y / x)``, in radians. The result is between ``-pi`` and ``pi``.
   The vector in the plane from the origin to point ``(x, y)`` makes this angle
   with the positive X axis. The point of :func:`atan2` is that the signs of both
   inputs are known to it, so it can compute the correct quadrant for the angle.
   For example, ``atan(1)`` and ``atan2(1, 1)`` are both ``pi/4``, but ``atan2(-1,
   -1)`` is ``-3*pi/4``.
    """
    pass

@native_c("__atan",["csrc/math/atan.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def atan(x):
    """
.. function:: atan(x)

   Return the arc tangent of *x*, in radians.
    """
    pass

@native_c("__acos",["csrc/math/acos.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def acos(x):
    """
.. function:: acos(x)

   Return the arc cosine of *x*, in radians.

    """
    pass

@native_c("__asin",["csrc/math/asin.c","csrc/math/scalbn.c","csrc/math/stdlib_trig.c"])
def asin(x):
    """
.. function:: asin(x)

   Return the arc sine of *x*, in radians.

    """
    pass

def degrees(rad):
    """
.. function:: degress(rad)

   Converts *rad* from radians to degrees.
    
    """
    return 180.0*rad/pi

def radians(degree):
    """
.. function:: radians(degree)

   Converts *degree* from degrees to radians.
    
    """
    return pi*degree/180.0

##### EXPONENTS

@native_c("__expn",["csrc/math/exp.c","csrc/math/stdlib_exp.c"])
def exp(x):
    """
.. function:: exp(x)

   Return ``e**x``.
    """
    pass

@native_c("__logn",["csrc/math/log.c","csrc/math/stdlib_exp.c"])
def log(x,base=-1):
    """
.. function:: log(x[, base])

   With one argument or with *base* non positive, return the natural logarithm of *x* (to base *e*).

   With two arguments, return the logarithm of *x* to the given *base*,
   calculated as ``log(x)/log(base)``.

    """
    pass

@native_c("__pow",["csrc/math/pow.c","csrc/math/stdlib_pow.c"])
def pow(x,y):
    """
.. function:: pow(x, y)

   Return ``x`` raised to the power ``y``.  

   Unlike the built-in ``**`` operator, :func:`math.pow` converts both
   its arguments to type :class:`float`.  Use ``**`` or the built-in
   :func:`pow` function for computing exact integer powers.
    """
    pass

@native_c("__sqrt",["csrc/math/sqrt.c","csrc/math/stdlib_pow.c"])
def sqrt(x):
    """
.. function:: sqrt(x)

   Return the square root of *x*.
    """
    pass

##### MISC

@native_c("__isnan",["csrc/math/isnan.c","csrc/math/stdlib_misc.c"])
def isnan(x):
    """
.. function:: isnan(x)

   Return ``True`` if *x* is a NaN (not a number), and ``False`` otherwise.

    """
    pass

@native_c("__isinf",["csrc/math/finite.c","csrc/math/stdlib_misc.c"])
def isinf(x):
    """
.. function:: isinf(x)

   Return ``True`` if *x* is a positive or negative infinity, and
   ``False`` otherwise.
    """
    pass

@native_c("__floor",["csrc/math/floor.c","csrc/math/stdlib_misc.c"])
def floor(x):
    """
.. function:: floor(x)

   Return the floor of *x*, the largest integer less than or equal to *x*.
    """
    pass

@native_c("__ceil",["csrc/math/ceil.c","csrc/math/stdlib_misc.c"])
def ceil(x):
    """
.. function:: ceil(x)

   Return the ceiling of *x*, the smallest integer greater than or equal to *x*.    
    """
    pass



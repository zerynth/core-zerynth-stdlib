"""
.. module:: datetime

*************
Date and time
*************

This module supplies classes for manipulating dates, times, and deltas.
It represents a minimalistic implementation of Python module
`datetime <https://docs.python.org/3/library/datetime.html>`_.

:class:`datetime` objects may be categorized as “aware” or “naive”
depending on whether or not they include timezone information.
An aware object can locate itself relative to other aware objects. An
*aware* object represents a specific moment in time that is not open to
interpretation.

A *naive* object does not contain enough information to unambiguously
locate itself relative to other :class:`datetime` objects. Whether a naive
object represents Coordinated Universal Time (UTC), local time, or time
in some other timezone is purely up to the program, just like it is up
to the program whether a particular number represents metres, miles, or
mass. Naive objects are easy to understand and to work with, at the cost
of ignoring some aspects of reality.

For applications requiring aware objects, :class:`datetime` objects have
an optional time zone information attribute, *tzinfo*, that can be set to
an instance of a :class:`timezone` class. These objects capture
information about the offset from UTC time and the time zone name.

The following classes are provided:

* :class:`timedelta`
* :class:`timezone`
* :class:`datetime`


Python's classes `date()
<https://docs.python.org/3/library/datetime.html#date-objects>`_ and `time()
<https://docs.python.org/3/library/datetime.html#time-objects>`_
are mimicked with a class :class:`datetime` set to midnight, and a class
:class:`timedelta` in the range [0-24h).


timedelta Objects
=================

A :class:`timedelta` object represents a duration, the difference between two
dates or times. With respect to the Python module
`datetime <https://docs.python.org/3/library/datetime.html>`_,
this implementation is constrained as follows:

    * Minimum resolution is *1 second*, instead of *1 microsecond*.
    * Arithmetic is done via direct function calls (:func:`add` vs
      :func:`__add__`) due to Zerynth's limits.


Class attributes
----------------

.. attribute:: timedelta.MINYEAR

   The year of :attr:`timedelta.min`, i.e.
   ``timedelta.min.tuple()[1] // (365×24×60×60) == -34``.


.. attribute:: timedelta.MAXYEAR

   The year of :attr:`timedelta.max`, i.e.
   ``timedelta.max.tuple()[1] // (365×24×60×60) == 34``.


.. attribute:: timedelta.min

   The most negative :class:`timedelta` object, ``timedelta(-2**30)``.


.. attribute:: timedelta.max

   The most positive :class:`timedelta` object, ``timedelta(2**30 - 1)``.


.. attribute:: timedelta.resolution

   The smallest possible difference between non-equal :class:`timedelta`
   objects, ``timedelta(seconds=1)``.


Class methods
-------------

.. class:: timedelta(hours=0, minutes=0, seconds=0, days=0, weeks=0)

All arguments are optional and default to ``0``. Arguments may be integers
or floats, and may be positive or negative. Only seconds are stored
internally. Arguments are converted to those units:

    * A minute is converted to 60 seconds.
    * An hour is converted to 3600 seconds.
    * A week is converted to 7 days.

If no argument is a float, the conversion and normalization processes are
exact (no information is lost).


.. method:: total_seconds()

   Return the total number of seconds contained in the duration.


.. method:: add(other)

   Return a :class:`timedelta` which represents the sum of two durations.


.. method:: sub(other)

   Return a :class:`timedelta` which represents the difference between
   two durations.


.. method:: mul(other)

   Return a delta multiplied by an integer or float. The result is rounded to
   the nearest second using round-half-to-even.


.. method:: truediv(other)

   When *other* is a float or an integer, returns a delta divided by *other*.
   The result is rounded to the nearest multiple of timedelta.resolution
   using round-half-to-even.

   When *other* is a delta, division of overall duration by interval unit
   *other*. Returns a float object.


.. method:: floordiv(other)

   The floor is computed and the remainder (if any) is thrown away. When
   *other* is a delta, an integer is returned.


.. method:: mod(other)

   The remainder is computed as a :class:`timedelta` object.


.. method:: divmod(other)

   Computes the quotient and the remainder: ``q = td1.floordiv(td2)`` and
   ``r = td1.mod(td2)``.  ``q`` is an integer and ``r`` is a :class:`timedelta`
   object.


.. method:: neg()

   Equivalent to ``td1.mul(-1)``.


.. method:: eq(other)

   Equivalent to ``td1.total_seconds() == td2.totalseconds()``.


.. method:: le(other)

   Equivalent to ``td1.total_seconds() <= td2.totalseconds()``.


.. method:: lt(other)

   Equivalent to ``td1.total_seconds() < td2.totalseconds()``.


.. method:: ge(other)

   Equivalent to ``td1.total_seconds() >= td2.totalseconds()``.


.. method:: gt(other)

   Equivalent to ``td1.total_seconds() > td2.totalseconds()``.


.. method:: bool()

   Return ``False`` when duration is ``0``.


.. method:: abs()

   Return a positive delta.


.. method:: isoformat()

   This method mimics Python's `isoformat()
   <https://docs.python.org/3/library/datetime.html#datetime.time.isoformat>`_
   for *time* objects by returning a string in the format ``HH:MM:SS``, where
   ``HH``, ``MM``, and ``SS`` are two digits of the time delta's hours,
   minutes and seconds, respectively, since midnight.  This is only if value
   is within the range [0-24h).

   For other values, it returns the format ``±Dd HH:MM:SS``, where ``±`` is
   the sign of the delta and ``D`` its number of days. This is *not* ISO
   compliant, but provides a complete representation.


.. method:: tuple(sign_pos='')

   Return the tuple ``(sign, days, hours, minutes, seconds)``, where ``sign`` is
   ``-`` if delta is negative, *sign_pos* otherwise.


Examples of usage
-----------------

An example of normalization::

    import datetime.timedelta

    # Components of another_year add up to exactly 365 days
    year = timedelta(days=365)
    another_year = timedelta(weeks=40, days=84, hours=23, minutes=50, seconds=600)
    print(year.eq(another_year)) # True
    print(year.total_seconds())  # 31536000


Examples of timedelta arithmetic::

    import datetime.timedelta

    year = timedelta(days=365)
    ten_years = year.mul(10)
    print(ten_years)                    # 3650d 00:00:00
    nine_years = ten_years.sub(year)
    print(nine_years)                   # 3285d 00:00:00
    three_years = nine_years.floordiv(3)
    print(three_years)                  # 1095d 00:00:00


timezone Objects
================

The :class:`timezone` class represents a timezone defined by a fixed
offset from UTC. Define a subclass of :class:`timezone` to capture
information about a particular time zone.

An instance of :class:`timezone` can be passed to the constructors for
:class:`datetime`. The latter objects view their attributes as being in
local time, and the :class:`timezone` object supports methods revealing
offset of local time from UTC, the name of the time zone, and DST offset,
all relative to a date-time object passed to them.


Methods to customize
--------------------

A subclass of :class:`timezone` may need to override the following methods.
Exactly which methods are needed depends on the uses made of aware
:class:`datetime` objects. If in doubt, simply implement all of them.


.. method:: utcoffset(dt)

   Return offset of local time from UTC, as a :class:`timedelta` object
   that is positive east of UTC. If local time is west of UTC, this should
   be negative.

   This represents the *total* offset from UTC; for example, if a
   :class:`timezone` object represents both time zone and DST adjustments,
   :meth:`timezone.utcoffset` should return their sum. If the UTC offset
   isn’t known, return ``None``. Else the value returned must be a
   :class:`timedelta` object strictly between ``timedelta(hours=-24)`` and
   ``timedelta(hours=24)`` (the magnitude of the offset must be less than one
   day). Most implementations of :meth:`timezone.utcoffset` will probably
   look like one of these two:

        return CONSTANT                 # fixed-offset class
        return CONSTANT + self.dst(dt)  # daylight-aware class

   If :meth:`timezone.utcoffset` does not return ``None``, :meth:`timezone.dst`
   should not return None either.

   The default implementation of :meth:`timezone.utcoffset` returns the sum
   of time zone and DST adjustments, if available.

.. method:: dst(dt)

   Return the daylight saving time (DST) adjustment, as a :class:`timedelta`
   object or ``None`` if DST information isn’t known.

   Return ``timedelta(0)`` if DST is not in effect. If DST is in effect, return
   the offset as a :class:`timedelta` object (see :meth:`timezone.utcoffset`
   for details). Note that DST offset, if applicable, has already been added
   to the UTC offset returned by :meth:`timezone.utcoffset`, so there’s no
   need to consult :meth:`timezone.dst` unless you’re interested in obtaining
   DST info separately.

   Most implementations of :meth:`timezone.dst` will probably look like one
   of these two::

       def dst(self, dt):
           # a fixed-offset class:  doesn't account for DST
           return timedelta(0)

   or::

       def dst(self, dt):
           # Code to set dston and dstoff to the time zone's DST
           # transition times based on the input *dt*'s year, and
           # expressed in standard local time.

           dt_ = dt.replace(tzinfo=None)
           if dt_.ge(dston) and dt_.lt(dstoff):
               return timedelta(hours=1)
           else:
               return timedelta(0)

   The default implementation of :meth:`timezone.dst` returns ``None``.


.. method:: tzname(dt)

   Return the time zone name corresponding to the :class:`datetime` object
   *dt*, as a string. Nothing about string names is defined by the
   :class:`datetime` module, and there’s no requirement that it mean anything
   in particular. For example, “GMT”, “UTC”, “-500”, “-5:00”, “EDT”,
   “US/Eastern”, “America/New York” are all valid replies. Return ``None`` if
   a string name isn’t known. Note that this is a method rather than a fixed
   string primarily because some :class:`timezone` subclasses will wish to
   return different names depending on the specific value of *dt* passed,
   especially if the :class:`timezone` class is accounting for daylight time.

   The default implementation of :meth:`timezone.tzname` returns the fixed
   value specified when the :class:`timezone` instance is constructed.
   If *name* is not provided in the constructor, the name returned by
   ``tzname()`` is generated from the value of the ``offset`` as follows.
   If *offset* is ``timedelta(0)``, the name is “UTC”, otherwise it returns
   the string provided by :meth:`timezone.isoformat` method.

These methods are called by a :class:`datetime` object, in response to their
methods of the same names. A :class:`datetime` object passes *self* as *dt*
argument.


Class attributes
----------------

.. attribute:: timezone.utc

   The UTC timezone, ``timezone(timedelta(0))``.


Class methods
-------------

.. class:: timezone(offset, name=None)

   The *offset* argument must be specified as a :class:`timedelta`
   object representing the difference between the local time and UTC.
   It must be strictly between ``timedelta(hours=-24)`` and
   ``timedelta(hours=24)``, otherwise :exc:`ValueError` is raised.

   The *name* argument is optional. If specified it must be a string
   that will be used as the value returned by the :meth:`datetime.tzname`
   method.


.. method:: isoformat(dt)

   Return a string in the format ``UTC±HH:MM``, where ``±`` is the sign of
   *offset* from UTC, ``HH`` and ``MM`` are two digits of offset's hours and
   offset's minutes respectively. If *offset* is ``timedelta(0)``, “UTC”
   is returned.

   If *utc* is ``False``, this method always returns ``±HH:MM``.

   *dt* is needed in determining the right offset; it can be ``None``.


Examples of usage
-----------------

`Central European Time <https://en.wikipedia.org/wiki/Summer_time_in_Europe>`_
(CET), used in most parts of Europe and a few North African countries, is a
standard time which is 1 hour ahead of Coordinated Universal Time (UTC).
As of 2011, all member states of the European Union observe summer time;
those that during the winter use CET use Central European Summer Time (CEST)
(or: UTC+02:00, daylight saving time) in summer (from last Sunday of March
to last Sunday of October). ::

    import datetime

    class cet(datetime.timezone):
        def __init__(self):
            datetime.timezone.__init__(self, datetime.timedelta(hours=1))

        def dst(self, dt):
            return datetime.timedelta(hours=1) if self.isdst(dt) else datetime.timedelta(0)

        def tzname(self, dt):
            return 'CEST' if self.isdst(dt) else 'CET'

        def isdst(self, dt):
            if dt is None:
                return False
            dt_ = dt.replace(tzinfo=None)
            year = dt.tuple()[0]
            day = 31 - (5*year//4 + 4) % 7 # last Sunday of March
            dst = dt_.replace(month=3, day=day)
            if dt_.lt(dst):
                return False
            day = 31 - (5*year//4 + 1) % 7 # last Sunday of October
            dst = dt_.replace(month=10, day=day)
            if dt_.lt(dst):
                return True
            return False

    tz = cet()
    print(tz.isoformat(datetime(2011, 1, 1))) # UTC+01:00
    print(tz.tzname   (datetime(2011, 1, 1))) # CET
    print(tz.isoformat(datetime(2011, 8, 1))) # UTC+02:00
    print(tz.tzname   (datetime(2011, 8, 1))) # CEST


datetime Objects
================

A :class:`datetime` object is a single object containing all the information
for specifying an absolute date and time point.

:class:`datetime` assumes the current Gregorian calendar extended in both
directions, past and future. January 1 of year 1 is called day number 1,
January 2 of year 1 is called day number 2, and so on.

:class:`datetime` assumes there are exactly 3600*24 seconds in every day and
subject to adjustment via a :class:`timezone` object.


Constructors
------------

.. class:: datetime(self, year, month, day, hour=0, minute=0, second=0, tzinfo=None)

   The *year*, *month* and *day* arguments are required. *tzinfo* may be
   ``None``, or an instance of a :class:`timezone` class. The remaining
   arguments must be integers in the following ranges:

   * ``MINYEAR <= year <= MAXYEAR``,
   * ``1 <= month <= 12``,
   * ``1 <= day <= number of days in the given month and year``,
   * ``0 <= hour < 24``,
   * ``0 <= minute < 60``,
   * ``0 <= second < 60``,

   If an argument outside those ranges is given, :exc:`ValueError` is raised.


.. function:: fromisoformat(date_string)

   Return a :class:`datetime` corresponding to a *date_string* in the format
   emitted by :meth:`datetime.isoformat`.

   Specifically, this function supports strings in the format::

       YYYY-MM-DD[*HH[:MM[:SS[.fff[fff]]]][+HH:MM[:SS[.ffffff]]]]

   where ``*`` can match any single character.


.. function:: fromordinal(n)

   Return the :class:`datetime` corresponding to the proleptic Gregorian
   ordinal, where January 1 of year 1 has ordinal 1. :exc:`ValueError` is
   raised unless ``1 <= ordinal <= datetime.max.toordinal()``. The hour,
   minute and second of the result are all 0, and *tzinfo* is ``None``.


.. function:: combine(date, time, tzinfo)

   Return a new :class:`datetime` object whose date components are equal to
   the given *date* object’s (see :meth:`datetime.date`), and whose time
   components are equal to the given *time* object’s (see
   :meth:`datetime.time`). If the *tzinfo* argument is provided, its value
   is used to set the *tzinfo* attribute of the result, otherwise the
   *tzinfo* attribute of the *date* argument is used.


Class attributes
----------------

.. attribute:: datetime.MINYEAR

   The smallest year number allowed in a :class:`datetime` object.
   :attr:`datetime.MINYEAR` is ``1``.


.. attribute:: datetime.MAXYEAR

   The largest year number allowed in a :class:`datetime` object.
   :attr:`datetime.MAXYEAR` is ``9999``.


Class methods
-------------

.. method:: add(other)

   In the expression ``datetime2 = datetime1.add(timedelta)``, ``datetime2``
   is a duration of ``timedelta`` removed from ``datetime1``, moving forward
   in time if ``timedelta > 0``, or backward if ``timedelta < 0``. The result
   has the same :class:`timezone` attribute as the input ``datetime1``, and
   ``datetime2 - datetime1 == timedelta`` after.

   Note that no time zone adjustments are done even if the input is an aware
   object.


.. method:: sub(other)

   If *other* is an instance of :class:`timedelta`, the expression
   ``datetime2 = datetime1.sub(timedelta)`` computes the ``datetime2`` such that
   ``datetime2 + timedelta == datetime1``. As for addition, the result has the
   same :class:`timezone` attribute as the input ``datetime1``, and no time
   zone adjustments are done even if the input is aware.

   If *other* is an instance of :class:`datetime`, subtraction
   ``timedelta = datetime2.sub(datetime1)`` is defined only if both operands
   are naive, or if both are aware. If one is aware and the other is naive,
   :exc:`TypeError` is raised.

   If both are naive, or both are aware and have the same :class:`timezone`
   attribute, the :class:`timezone` attributes are ignored, and the result
   is a :class:`timedelta` object *t* such that ``datetime2 + t == datetime1``.
   No time zone adjustments are done in this case.

   If both are aware and have different :class:`timezone` attributes, ``a-b``
   acts as if *a* and *b* were first converted to naive UTC datetimes first.


.. method:: lt(other)

   Equivalent to ``dt1.toordinal() < dt2.toordinal()``.


.. method:: lt(other)

   Equivalent to ``dt1.toordinal() <= dt2.toordinal()``.


.. method:: lt(other)

   Equivalent to ``dt1.toordinal() == dt2.toordinal()``.


.. method:: lt(other)

   Equivalent to ``dt1.toordinal() >= dt2.toordinal()``.


.. method:: lt(other)

   Equivalent to ``dt1.toordinal() > dt2.toordinal()``.


.. method:: utcoffset()

   If *tzinfo* is ``None``, returns ``None``, else returns a
   :class:`timedelta` object with magnitude less than one day.


.. method:: replace(year=None, month=None, day=None, hour=None, minute=None, second=None, tzinfo=True)

   Return a :class:`datetime` with the same attributes, except for those
   attributes given new values by whichever keyword arguments are specified.
   Note that ``tzinfo=None`` can be specified to create a naive
   :class:`datetime` from an aware :class:`datetime` with no conversion of
   date and time data.


.. method:: astimezone(tz)

   Return a :class:`datetime` object with new *tzinfo* attribute
   *tz*, adjusting the date and time data so the result is the same UTC time
   as *self*, but in *tz*’s local time. *self* must be aware.

   If you merely want to attach a :class:`timezone` object *tz* to a
   :class:`datetime` *dt* without adjustment of date and time data, use
   ``dt.replace(tzinfo=tz)``. If you merely want to remove the :class:`timezone`
   object from an aware :class:`datetime` *dt* without conversion of date and
   time data, use ``dt.replace(tzinfo=None)``.


.. method:: isoformat(sep='T')

   Return a string representing the date and time in ISO 8601 format
   ``YYYY-MM-DDTHH:MM:SS``. If :meth:`datetime.utcoffset` does not return
   ``None``, a string is appended, giving the UTC offset:
   ``YYYY-MM-DDTHH:MM:SS+HH:MM``.


.. method:: date()

   Return a :class:`datetime` instance whose date and time zone components
   are equal to the input object and time is set to midnight.


.. method:: time()

   Return a :class:`timedelta` instance whose time components are equal to
   the input object.


.. method:: toordinal()

   Return the proleptic Gregorian ordinal of the date.


.. method:: isoweekday()

   Return the day of the week as an integer, where Monday is 1 and Sunday
   is 7. For example, ``date(2002, 12, 4).isoweekday() == 3``, a Wednesday.


.. method:: tuple()

   Return the tuple ``(year, month, day, hour, minute, second, tzinfo)``.


Examples of usage
-----------------

Examples of working with :class:`datetime` objects::

    from datetime import timedelta, timezone, datetime, fromisoformat

    print(datetime(2005, 7, 14, 12, 30))            # 2005-07-14 12:30:00
    dt = fromisoformat('2006-11-21 16:30+01:00')
    print(dt.add(timedelta(hours=23)))              # 2006-11-22 15:30:00+01:00
    tz1 = timezone(timedelta(hours=4, minutes=30))
    print(tz1)                                      # UTC+04:30
    dt = datetime(1900, 11, 21, 3, 30, tzinfo=tz1)
    print(dt)                                       # 1900-11-21 03:30:00+04:30
    print(dt.astimezone(timezone.utc))              # 1900-11-20 23:00:00+00:00

"""

#-if 0
import builtins as __builtins__
#-endif

# The following functions were (stolen and) adapted from Python's datetime.
def _is_leap(year):
    return year % 4 == 0 and (year % 100 != 0 or year % 400 == 0)

def _days_before_year(year):
    # year -> number of days before January 1st of year.
    y = year - 1
    return y*365 + y//4 - y//100 + y//400

def _days_in_month(year, month):
    # year, month -> number of days in that month in that year.
    if month == 2 and _is_leap(year):
        return 29
    return (0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31)[month]

def _days_before_month(year, month):
    # year, month -> number of days in year preceding first day of month.
    return (0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334)[month]\
         + (month > 2 and _is_leap(year))

def _ymd2ord(year, month, day):
    # year, month, day -> ordinal, considering 01-Jan-0001 as day 1.
    return _days_before_year(year) + _days_before_month(year, month) + day

def _ord2ymd(n):
    # ordinal -> (year, month, day), considering 01-Jan-0001 as day 1.
    n -= 1
    n400, n = divmod(n, 146097)
    year = n400 * 400 + 1
    n100, n = divmod(n, 36524)
    n4, n = divmod(n, 1461)
    n1, n = divmod(n, 365)
    year += n100 * 100 + n4 * 4 + n1
    if n1 == 4 or n100 == 4:
        return year-1, 12, 31
    month = (n + 50) >> 5
    preceding = _days_before_month(year, month)
    if preceding > n:
        month -= 1
        preceding -= _days_in_month(year, month)
    n -= preceding
    return year, month, n+1


class timedelta:
    MINYEAR = -34
    MAXYEAR = 34

    def __init__(self, hours=0, minutes=0, seconds=0, days=0, weeks=0):
        self._s = round((((weeks * 7 + days) * 24 + hours) * 60 + minutes) * 60 + seconds)

    def total_seconds(self):
        return self._s

    def add(self, other):
        return timedelta(seconds=self._s + other._s)

    def sub(self, other):
        return timedelta(seconds=self._s - other._s)

    def mul(self, other):
        return timedelta(seconds=round(other * self._s))

    def truediv(self, other):
        if isinstance(other, timedelta):
           return self._s / other._s
        else:
           return timedelta(seconds=round(self._s / other))

    def floordiv(self, other):
        if isinstance(other, timedelta):
            return self._s // other._s
        else:
            return timedelta(seconds=int(self._s // other))

    def mod(self, other):
        return timedelta(seconds=self._s % other._s)

    def divmod(self, other):
        q, r = __builtins__.divmod(self._s, other._s)
        return q, timedelta(seconds=r)

    def neg(self):
        return timedelta(seconds=-self._s)

    def eq(self, other):
        return self._s == other._s

    def le(self, other):
        return self._s <= other._s

    def lt(self, other):
        return self._s < other._s

    def ge(self, other):
        return self._s >= other._s

    def gt(self, other):
        return self._s > other._s

    def bool(self):
        return self._s != 0

    def abs(self):
        return timedelta(seconds=__builtins__.abs(self._s))

    def __str__(self):
        return self.isoformat()

    def isoformat(self):
        t = self.tuple()
        if 0 <= self._s < 86400:
            return "%02d:%02d:%02d" % t[2:]
        else:
            return "%s%dd %02d:%02d:%02d" % t

    def tuple(self, sign_pos=''):
        s = self._s
        if s < 0:
            s *= -1
            g = '-'
        else:
            g = sign_pos
        m, s = __builtins__.divmod(s, 60)
        h, m = __builtins__.divmod(m, 60)
        d, h = __builtins__.divmod(h, 24)
        return g, d, h, m, s

timedelta.min = timedelta(seconds=-1073741824) # -2**30
timedelta.max = timedelta(seconds= 1073741823) #  2**30 - 1
timedelta.resolution = timedelta(seconds=1)


class timezone:
    def __init__(self, offset, name=None):
        if not (timedelta(hours=-24).lt(offset) and offset.lt(timedelta(hours=24))):
            raise ValueError
        self._offset = offset
        self._name = name

    def __str__(self):
        return self.tzname(None)

    def utcoffset(self, dt):
        dst = self.dst(dt)
        if dst is None or not dst.bool():
            return self._offset
        return self._offset.add(dst)

    def dst(self, dt):
        return None

    def tzname(self, dt):
        if self._name != None:
            return self._name
        return self.isoformat(dt)

    def isoformat(self, dt, *, utc=True):
        td = self.utcoffset(dt)
        if utc and not td.bool():
            return 'UTC'
        sign, day, hour, minute, second = td.tuple('+')
        return '%s%s%02d:%02d' % ('UTC' if utc else '', sign, hour, minute)

timezone.utc = timezone(timedelta(0))


class datetime:
    MINYEAR = 1
    MAXYEAR = 9999

    def __init__(self, year, month, day, hour=0, minute=0, second=0, tzinfo=None):
        if year == 0 and month == 0 and day > 0:
            self._ord = day
        elif self.MINYEAR <= year <= self.MAXYEAR\
        and  1 <= month  <= 12\
        and  1 <= day    <= _days_in_month(year, month)\
        and  0 <= hour   <  24\
        and  0 <= minute <  60\
        and  0 <= second <  60:
            self._ord = _ymd2ord(year, month, day)
        else:
            raise ValueError
        self._time = timedelta(hour, minute, second)
        self._tz = tzinfo

    def add(self, other):
        time = self._time.add(other)
        sign, days, hour, minute, second = time.tuple()
        if sign == '-':
            days -= 1
            time = time.add(timedelta(days=-days))
        year, month, day, hour, minute, second, tz\
                = self._tuple(self._ord + days, time, self._tz)[:7]
        return datetime(year, month, day, hour, minute, second, tz)

    def sub(self, other):
        if isinstance(other, timedelta):
            return self.add(other.neg())
        elif isinstance(other, datetime):
            days, time = self._sub(other)
            return time.add(timedelta(days=days))
        else:
            raise TypeError

    def lt(self, other):
        return self._cmp(other) < 0

    def le(self, other):
        return self._cmp(other) <= 0

    def eq(self, other):
        return self._cmp(other) == 0

    def ge(self, other):
        return self._cmp(other) >= 0

    def gt(self, other):
        return self._cmp(other) > 0

    def __str__(self):
        return self.isoformat(' ')

    def utcoffset(self):
        return None if self._tz is None else self._tz.utcoffset(self)

    def dst(self):
        return None if self._tz is None else self._tz.dst(self)

    def date(self):
        return datetime(0, 0, self.toordinal(), tzinfo=self._tz)

    def time(self):
        return timedelta(seconds=self._time.total_seconds())

    def tzname(self):
        return None if self._tz is None else self._tz.tzname(self)

    def replace(self, year=None, month=None, day=None, hour=None,
                minute=None, second=None, tzinfo=True):
        year_, month_, day_, hour_, minute_, second_, tz_\
                = self.tuple()
        if year   is None: year   = year_
        if month  is None: month  = month_
        if day    is None: day    = day_
        if hour   is None: hour   = hour_
        if minute is None: minute = minute_
        if second is None: second = second_
        if tzinfo is True: tzinfo = tz_
        return datetime(year, month, day,
                        hour, minute, second,
                        tzinfo)

    def astimezone(self, tz):
        if self._tz == None:
            raise NotImplementedError
        return self.sub(self.utcoffset())\
                   .add(tz.utcoffset(self))\
                   .replace(tzinfo=tz)

    def dateisoformat(self):
        return self.isoformat()[:10]

    def timeisoformat(self):
        return self.isoformat()[11:19]

    def isoformat(self, sep='T'):
        dt = ('%04d-%02d-%02d'+sep+'%02d:%02d:%02d') % self.tuple()[:6]
        if self._tz == None:
            return dt
        return dt + self._tz.isoformat(self, utc=False)

    def toordinal(self):
        return self._ord

    def isoweekday(self):
        return self._ord % 7 or 7

    def tuple(self):
        return self._tuple(self._ord, self._time, self._tz)[:-2]

    def _sub(self, other):
        # Subtract two datetime instances.
        if (self._tz == None) ^ (other._tz == None):
               raise TypeError

        if self._tz == None or self.utcoffset().eq(other.utcoffset()):
            dt1 = self
            dt2 = other
        else:
            dt1 = self .astimezone(timezone.utc)
            dt2 = other.astimezone(timezone.utc)

        days = dt1._ord - dt2._ord
        time = dt1._time.sub(dt2._time)
        return days, time

    def _cmp(self, other):
        # Compare two datetime instances.
        days, time = self._sub(other)
        if days < 0: return -1
        if days > 0: return  1

        secs = time.total_seconds()
        if secs < 0: return -1
        if secs > 0: return  1

        return 0

    def _tuple(self, ordinal, time, tz):
        # Split a datetime to its components.
        year, month, day = _ord2ymd(ordinal)
        sign, days, hour, minute, second = time.tuple()
        return year, month, day, hour, minute, second, tz, days, sign

def fromisoformat(s):
    l = len(s)
    if l < 10 or s[4] != '-' or s[7] != '-':
        raise ValueError
    # parse date
    year = int(s[0:4])
    month = int(s[5:7])
    day = int(s[8:10])
    hour = 0
    minute = 0
    second = 0
    microsecond = 0
    tz_sign = ''
    tz_hour = 0
    tz_minute = 0
    tz_second = 0
    tz_microsecond = 0
    i = 10
    if l > i and s[i] != '+':
        # parse time
        if l - i < 3:
            raise ValueError
        i += 3
        hour = int(s[i-2:i])
        if l > i and s[i] == ':':
            i += 3
            if l - i < 0:
                raise ValueError
            minute = int(s[i-2:i])
            if l > i and s[i] == ':':
                i += 3
                if l - i < 0:
                    raise ValueError
                second = int(s[i-2:i])
                if l > i and s[i] == '.':
                    i += 4
                    if l - i < 0:
                        raise ValueError
                    microsecond = 1000*int(s[i-3:i])
                    if l > i and s[i] != '+':
                        i += 3
                        if l - i < 0:
                            raise ValueError
                        microsecond += int(s[i-3:i])
    if l > i:
        if s[i] not in '+-':
            raise ValueError
        tz_sign = s[i]
        i += 6
        if l - i < 0:
            raise ValueError
        tz_hour = int(s[i-5:i-3])
        tz_minute = int(s[i-2:i])
        if l > i and s[i] == ':':
            i += 3
            if l - i < 0:
                raise ValueError
            tz_second = int(s[i-2:i])
            if l > i and s[i] == '.':
                i += 7
                if l - i < 0:
                    raise ValueError
                tz_microsecond = int(s[i-6:i])
    if l != i:
        raise ValueError
    if tz_sign:
        td = timedelta(tz_hour, tz_minute, tz_second)
        if tz_sign == '-':
            td = td.neg()
        tz = timezone(td)
    else:
        tz = None
    return datetime(year, month, day, hour, minute, second, tz)

def fromordinal(n):
    if not 1 <= n <= 3652059:
        raise ValueError
    return datetime(0, 0, n)

def combine(date, time, tzinfo=True):
    if tzinfo is True:
        dt = date
    else:
        dt = date.replace(tzinfo=tzinfo)
    return dt.add(time)

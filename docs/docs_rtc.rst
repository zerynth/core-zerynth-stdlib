.. module:: rtc

***************
Real-Time Clock
***************

This module loads the Real-Time Clock (rtc) driver of the embedded device.

When imported, automatically sets the system rtc driver to the default one.

    
.. function:: init(drvname)

    Loads the rtc driver identified by *drvname*

    Returns the previous driver without disabling it.

    
.. function:: set_utc(seconds, microseconds=0)

    :param seconds: integer Unix timestamp containing the total number of seconds from the 1st of January 1970 at UTC
    :param microseconds: integer number representing the microseconds part of the timestamp to reach sub-second precision

    Sets a Coordinated Universal Time (UTC) reference for the rtc.

    
.. class:: TimeInfo()

    Class containing useful time information to be filled by the :func:`get_utc` function.
    List of attributes:

        * :attr:`Timeinfo.tv_seconds`: Unix timestamp containing the total number of seconds from the 1st of January 1970 at UTC;
        * :attr:`Timeinfo.tv_microseconds`: number of microseconds to complete the timestamp with sub-second precision;

        * :attr:`Timeinfo.tm_year`: current year
        * :attr:`Timeinfo.tm_month`: months since January (0-11)
        * :attr:`Timeinfo.tm_mday`: day of the month (1-31)
        * :attr:`Timeinfo.tm_hour`: hours since midnight (0-23)
        * :attr:`Timeinfo.tm_min`: minutes after the hour (0-59)
        * :attr:`Timeinfo.tm_sec`: seconds after the minute (0-59)
        * :attr:`Timeinfo.tm_wday`: days since Sunday (0-6)
        * :attr:`Timeinfo.tm_yday`: days since January 1 (0-365)

    
.. function:: get_utc(verbosity=2)

    When called with verbosity parameter set to :samp:`2`, returns a :class:`TimeInfo` object filled with info derived from the rtc.
    Only :attr:`Timeinfo.tv_seconds` and :attr:`Timeinfo.tv_microseconds` are guaranteed to be filled correctly.
    The availability of the other fields depend on the underlying driver implementation.

    When called with verbosity parameter set to :samp:`1`, returns a tuple containing timestamp seconds and microseconds.

    When called with verbosity parameter set to :samp:`0`, returns a single integer representing the Unix timestamp.


    

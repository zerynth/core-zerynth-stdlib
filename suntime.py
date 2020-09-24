"""
.. module:: suntime

***********************
Sunrise and sunset time
***********************

Approximated calculation of sunrise and sunset time. Adapted from
https://github.com/SatAgro/suntime.


Class methods
-------------

.. class:: Sun(latitude, longitude)

   Arguments *latitude* and *longitude* are floats representing the
   coordinates of a place on Earth.


.. method:: get_sunrise_time(year, month, day)

   Calculate the sunrise time for the given date. It returns a tuple of
   integers ``(hour, minute)`` in UTC time or ``None`` if sun doesn't
   raise on that location at the given date.


.. method:: get_sunset_time(year, month, day)

   Calculate the sunset time for the given date. It returns a tuple of
   integers ``(hour, minute)`` in UTC time or ``None`` if sun doesn't
   set on that location at the given date.


Examples of usage
-----------------

This example calculates sunrise and sunset of three major cities::

    import suntime

    test('suntime: example')

    Rome     = suntime.Sun( 41.902782 , 12.496366 )
    Warsaw   = suntime.Sun( 51.21     , 21.01     )
    CapeTown = suntime.Sun(-33.9252192, 18.4240762)

    dt1 = (2000, 1, 1)
    sr1 = Rome.get_sunrise_time(*dt1) # (6, 38)
    ss1 = Rome.get_sunset_time (*dt1) # (15, 49)
    print('Rome:', sr1, ss1)

    dt2 = (2014, 10, 3)
    sr2 = Warsaw.get_sunrise_time(*dt2) # (4, 39)
    ss2 = Warsaw.get_sunset_time (*dt2) # (16, 10)
    print('Warsaw:', sr2, ss2)

    dt3 = (2016, 12, 21)
    sr3 = CapeTown.get_sunrise_time(*dt3) # (3, 32)
    ss3 = CapeTown.get_sunset_time (*dt3) # (17, 57)
    print('Cape Town:', sr3, ss3)

If :mod:`datetime` module is available, results above can be expressed
in local time::

    import datetime as datetimelib

    timedelta = datetimelib.timedelta
    timezone  = datetimelib.timezone
    datetime  = datetimelib.datetime

    utc = timezone.utc
    tz1 = timezone(timedelta(hours=1))
    tz2 = timezone(timedelta(hours=3))
    tz3 = timezone(timedelta(hours=2))

    # https://www.timeanddate.com/sun/italy/rome?month=1&year=2000
    print('Rome:')
    rt1 = datetime(*dt1, *sr1, tzinfo=utc).astimezone(tz1) # 2000-01-01 07:38:00+01:00
    st1 = datetime(*dt1, *ss1, tzinfo=utc).astimezone(tz1) # 2000-01-01 16:49:00+01:00
    print('>', rt1)
    print('>', st1)

    # https://www.timeanddate.com/sun/poland/warsaw?month=10&year=2014
    print('Warsaw:')
    rt2 = datetime(*dt2, *sr2, tzinfo=utc).astimezone(tz2) # 2014-10-03 07:39:00+03:00
    st2 = datetime(*dt2, *ss2, tzinfo=utc).astimezone(tz2) # 2014-10-03 19:10:00+03:00
    print('>', rt2)
    print('>', st2)

    # https://www.timeanddate.com/sun/south-africa/cape-town?month=12&year=2016
    print('Cape Town:')
    rt3 = datetime(*dt3, *sr3, tzinfo=utc).astimezone(tz3) # 2016-12-21 05:32:00+02:00
    st3 = datetime(*dt3, *ss3, tzinfo=utc).astimezone(tz3) # 2016-12-21 19:57:00+02:00
    print('>', rt3)
    print('>', st3)
"""

import math

class Sun:
    def __init__(self, lat, lon):
        self._lat = lat
        self._lon = lon

    def get_sunrise_time(self, year, month, day):
        return self._calc_sun_time(year, month, day, True)

    def get_sunset_time(self, year, month, day):
        return self._calc_sun_time(year, month, day, False)

    def _calc_sun_time(self, year, month, day, isRiseTime, zenith=90.8):
        TO_RAD = math.pi/180.0

        # 1. first calculate the day of the year
        N1 = math.floor(275 * month / 9)
        N2 = math.floor((month + 9) / 12)
        N3 = (1 + math.floor((year - 4 * math.floor(year / 4) + 2) / 3))
        N = N1 - (N2 * N3) + day - 30

        # 2. convert the longitude to hour value and calculate an approximate time
        lngHour = self._lon / 15

        if isRiseTime:
            t = N + ((6 - lngHour) / 24)
        else: #sunset
            t = N + ((18 - lngHour) / 24)

        # 3. calculate the Sun's mean anomaly
        M = (0.9856 * t) - 3.289

        # 4. calculate the Sun's true longitude
        L = M + (1.916 * math.sin(TO_RAD*M)) + (0.020 * math.sin(TO_RAD * 2 * M)) + 282.634
        L = self._force_range(L, 360 ) #NOTE: L adjusted into the range [0,360)

        # 5a. calculate the Sun's right ascension

        RA = (1/TO_RAD) * math.atan(0.91764 * math.tan(TO_RAD*L))
        RA = self._force_range(RA, 360 ) #NOTE: RA adjusted into the range [0,360)

        # 5b. right ascension value needs to be in the same quadrant as L
        Lquadrant  = (math.floor( L/90)) * 90
        RAquadrant = (math.floor(RA/90)) * 90
        RA = RA + (Lquadrant - RAquadrant)

        # 5c. right ascension value needs to be converted into hours
        RA = RA / 15

        # 6. calculate the Sun's declination
        sinDec = 0.39782 * math.sin(TO_RAD*L)
        cosDec = math.cos(math.asin(sinDec))

        # 7a. calculate the Sun's local hour angle
        cosH = (math.cos(TO_RAD*zenith) - (sinDec * math.sin(TO_RAD*self._lat))) / (cosDec * math.cos(TO_RAD*self._lat))

        if cosH > 1:
            return None     # The sun never rises on this location (on the specified date)
        if cosH < -1:
            return None     # The sun never sets on this location (on the specified date)

        # 7b. finish calculating H and convert into hours

        if isRiseTime:
            H = 360 - (1/TO_RAD) * math.acos(cosH)
        else: #setting
            H = (1/TO_RAD) * math.acos(cosH)

        H = H / 15

        #8. calculate local mean time of rising/setting
        T = H + RA - (0.06571 * t) - 6.622

        #9. adjust back to UTC
        UT = T - lngHour
        UT = self._force_range(UT, 24)   # UTC time in decimal format (e.g. 23.23)

        #10. Return
        hr = self._force_range(int(UT), 24)
        min = round((UT - int(UT))*60)
        if min == 60:
            min = 0
            hr = self._force_range(hr + 1, 24)

        return hr, min

    def _force_range(self, v, max):
        # force v to be >= 0 and < max
        if v < 0:
            return v + max
        elif v >= max:
            return v - max

        return v
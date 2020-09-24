################################################################################
# Suntime Example
#
################################################################################

import suntime
import streams

streams.serial()

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

#######################################################################

# if `datetime` module is available
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
rt1 = datetime(dt1[0], dt1[1], dt1[2], sr1[0], sr1[1], tzinfo=utc).astimezone(tz1) # 2000-01-01 07:38:00+01:00
st1 = datetime(dt1[0], dt1[1], dt1[2], ss1[0], ss1[1], tzinfo=utc).astimezone(tz1) # 2000-01-01 16:49:00+01:00
print('>', rt1)
print('>', st1)

# https://www.timeanddate.com/sun/poland/warsaw?month=10&year=2014
print('Warsaw:')
rt2 = datetime(dt2[0], dt2[1], dt2[2], sr2[0], sr2[1], tzinfo=utc).astimezone(tz2) # 2014-10-03 07:39:00+03:00
st2 = datetime(dt2[0], dt2[1], dt2[2], ss2[0], ss2[1], tzinfo=utc).astimezone(tz2) # 2014-10-03 19:10:00+03:00
print('>', rt2)
print('>', st2)

# https://www.timeanddate.com/sun/south-africa/cape-town?month=12&year=2016
print('Cape Town:')
rt3 = datetime(dt3[0], dt3[1], dt3[2], sr3[0], sr3[1], tzinfo=utc).astimezone(tz3) # 2016-12-21 05:32:00+02:00
st3 = datetime(dt3[0], dt3[1], dt3[2], ss3[0], ss3[1], tzinfo=utc).astimezone(tz3) # 2016-12-21 19:57:00+02:00
print('>', rt3)
print('>', st3)
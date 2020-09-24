################################################################################
# Datetime Example
#
################################################################################

import streams
import datetime

streams.serial()
dtinc = datetime.fromisoformat('2006-12-21 16:30+01:00')
dtdec = datetime.fromisoformat('2006-12-21 16:30+01:00')

print("=======================================================")
while True:
    try:
        # datetime
        tz = datetime.timezone(datetime.timedelta(hours=2))
        a = datetime.datetime(2017, 11, 28, 23, 55, 59, tz)
        print("Datetime -->", a)
    
        # Datetime stamps
        year, month, day, hour, minute, second, tz, days, sign = a.tuple()
        print("year =", year)
        print("month =", month)
        print("day =", day)
        print("hour =", hour)
        print("minute =", minute)
        print("second =", second)
        print("tz =", tz)
    
        # from isoformat and add/sub timedelta
        print("-------------------------------------------------------")
        dtinc = dtinc.add(datetime.timedelta(days=1))
        dtdec = dtdec.sub(datetime.timedelta(days=1))
        print("increment 1 day -->", dtinc)
        print("decrement 1 day -->", dtdec)
        # timezone
        tz1 = datetime.timezone(datetime.timedelta(hours=4, minutes=30))
        print("timezone -->", tz1)                                                   # UTC+04:30
        dtz = datetime.datetime(1900, 11, 21, 3, 30, tzinfo=tz1)
        print(dtz)                                                                    # 1900-11-21 03:30:00+04:30
        print("as timezone -->", dtz.astimezone(datetime.timezone.utc))               # 1900-11-20 23:00:00+00:00
        print("=======================================================")
    except Exception as e:
        print(e)
    sleep(1000)

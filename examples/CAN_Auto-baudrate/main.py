# CAN Auto-baudrate
# Created at 2020-02-10 16:39:24.048614

from fortebit.polaris import polaris
import can

polaris.init()

print("CAN Bus Auto-Baudrate Example")

# Initialize CAN transceiver
pinMode(polaris.internal.PIN_CAN_STANDBY, OUTPUT)
digitalWrite(polaris.internal.PIN_CAN_STANDBY, LOW)
sleep(100)

def can_auto_baudrate():
    print("Start auto baudrate...")
    for bps in (1000000,800000,500000,250000,125000,100000,50000,10000):
        print("Try",bps,"bps")
        try:
            canbus = can.Can(CAN0, bps, options=can.OPTION_LISTEN_ONLY)
        except Exception as e:
            print(e)
            continue
        try:
            canbus.add_filter(0,0)
            canbus.receive(timeout=500)
            print("Found baudrate:",bps)
            canbus.done()
            return bps
        except TimeoutError:
            pass
        except Exception as e:
            print(e)
        print("Errors",canbus.get_errors())
        canbus.done()
    print("Auto baudrate failed!")
    return None

bps = None
while bps is None:
    bps = can_auto_baudrate()
    sleep(1000)

print("Start active listening at",bps,"bps")
canbus = can.Can(CAN0, bps)
canbus.add_filter(0,0)
while True:
    try:
        msg = canbus.receive()
        print("ID",hex(msg[0] & can.FRAME_EXT_MASK),"IDE",(msg[0] & can.FRAME_EXT_FLAG) != 0,"RTR",(msg[0] & can.FRAME_RTR_FLAG) != 0,"DLC",msg[1],"DATA",[hex(b) for b in msg[2]])
    except Exception as e:
        print(e)

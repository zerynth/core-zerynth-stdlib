# Can Bus Example
# Created at 2019-12-17 16:21:55.938660

from fortebit.polaris import polaris
import can

polaris.init()

print("CAN Bus Auto-Baudrate Example")

# Initialize CAN transceiver
pinMode(polaris.internal.PIN_CAN_STANDBY, OUTPUT)
digitalWrite(polaris.internal.PIN_CAN_STANDBY, LOW)
sleep(100)

canbus = can.Can(CAN0, 500000, options=OPTION_LOOPBACK)

def can_rx(timeout=-1):
    print("rx, listening...")
    msg = canbus.receive(timeout=timeout)
    print("ID",hex(msg[0] & can.FRAME_EXT_MASK),"IDE",(msg[0] & can.FRAME_EXT_FLAG) != 0,"RTR",(msg[0] & can.FRAME_RTR_FLAG) != 0,"DLC",msg[1],"DATA",[hex(b) for b in msg[2]])

def abort_rx():
    sleep(1000)
    print("abort rx")
    canbus.abort_receive()
    sleep(500)
    print("resume rx")
    canbus.resume_receive()
    
def abort_tx():
    sleep(1000)
    print("abort tx")
    canbus.abort_transmit()
    sleep(500)
    print("resume tx")
    canbus.resume_transmit()
    
f0 = canbus.add_filter(0,0)
f1 = canbus.add_filter(0x123|can.FRAME_EXT_FLAG,can.FRAME_EXT_MASK|can.FRAME_EXT_FLAG)
print("Filters",f0,f1)
canbus.del_filter(f0)
f2 = canbus.add_filter(0x456,can.FRAME_STD_MASK)
f3 = canbus.add_filter(0x88|can.FRAME_RTR_FLAG,can.FRAME_STD_MASK|can.FRAME_RTR_FLAG)
print("Filters",f1,f2,f3)

print("tx std data")
canbus.transmit(0x456,8,b'abcdefgh')

print("tx ext remote")
canbus.transmit(0x123|can.FRAME_EXT_FLAG|can.FRAME_RTR_FLAG,8)

print("Errors",canbus.get_errors())

thread(abort_rx)
try:
    can_rx()
except Exception as e:
    print(e)
sleep(1000)
can_rx()
canbus.done()


# test standby/wakeup
digitalWrite(polaris.internal.PIN_CAN_STANDBY, HIGH)
sleep(100)

sleeping = True
def wakeup():
    global sleeping
    if sleeping:
        sleeping = False
        onPinFall(CANRX0, None)
        print("CAN wakeup")

pinMode(CANRX0,INPUT_PULLUP)
onPinFall(CANRX0, wakeup)

print("Going to sleep...")
while sleeping:
    sleep(10)

digitalWrite(polaris.internal.PIN_CAN_STANDBY, LOW)


canbus = can.Can(CAN0, 500000)
f0 = canbus.add_filter(0,0)
print("Timeout rx 3s...")
try:
    can_rx(timeout=3000)
except Exception as e:
    print(e)

thread(abort_tx)
id=0
while id <= can.FRAME_STD_MASK:
    try:
        canbus.transmit((id<<18)|can.FRAME_EXT_FLAG|can.FRAME_RTR_FLAG,id&7)
        canbus.transmit(id+6,(id&7)+1,b'abcdefgh')
    except Exception as e:
        print(e)
        sleep(100)
    #sleep(50)
    id += 15

print("Errors",canbus.get_errors())

while True:
    can_rx()
    print("Errors",canbus.get_errors())

canbus.done()

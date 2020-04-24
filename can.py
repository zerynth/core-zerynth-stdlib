"""
.. module:: can

***
CAN
***

This module loads the Controller Area Network (CAN) driver.

A Controller Area Network (CAN bus) is a robust vehicle bus standard designed to allow microcontrollers and devices to communicate with each other's applications without a host computer.
It is a message-based protocol, designed originally for multiplex electrical wiring within automobiles to save on copper, but can also be used in many other contexts. 
For each device the data in a frame is transmitted sequentially but in such a way that if more than one device transmits at the same time the highest priority device is able to continue while the others back off.
Frames are received by all devices, including by the transmitting device.

    """


__define(_CANDRIVER_INIT, 0)
__define(_CANDRIVER_DONE, 1)
__define(_CANDRIVER_ERRORS, 2)
__define(_CANDRIVER_TIMING, 3)
__define(_CANDRIVER_RX, 4)
__define(_CANDRIVER_TX, 5)
__define(_CANDRIVER_ADDFILTER, 6)
__define(_CANDRIVER_DELFILTER, 7)
__define(_CANDRIVER_STOPRX, 8)
__define(_CANDRIVER_STOPTX, 9)
__define(_CANDRIVER_RESUMERX, 10)
__define(_CANDRIVER_RESUMETX, 11)

# special address description flags for the CAN_ID
FRAME_EXT_FLAG = 0x80000000  # EFF/SFF is set in the MSB
FRAME_RTR_FLAG = 0x40000000  # remote transmission request

# valid bits in CAN ID for frame formats
FRAME_STD_MASK = 0x000007FF  # standard frame format (SFF)
FRAME_EXT_MASK = 0x1FFFFFFF  # extended frame format (EFF)

# CAN error flags
ERROR_WARNING       = 0x0001  # Protocol Error Warning
ERROR_PASSIVE       = 0x0002  # Error Passive
ERROR_BUSOFF        = 0x0004  # Bus-off error
ERROR_STUFF         = 0x0008  # Stuff error
ERROR_FORM          = 0x0010  # Form error
ERROR_ACK           = 0x0020  # Acknowledgment error
ERROR_RECESSIVE     = 0x0040  # Bit recessive error
ERROR_DOMINANT      = 0x0080  # Bit dominant error
ERROR_CRC           = 0x0100  # CRC error
ERROR_RX_OVERRUN    = 0x0200  # Rx FIFO overrun error
ERROR_TX_ARBLOST    = 0x0400  # Tx failure due to arbitration lost
ERROR_TX_ERROR      = 0x0800  # Tx failure due to transmit error

# CAN configuration options
OPTION_NO_RETRY     = 0x01
OPTION_NO_AUTO_OFF  = 0x02
OPTION_KEEP_ORDER   = 0x04
OPTION_LISTEN_ONLY  = 0x08
OPTION_LOOPBACK     = 0x10


@native_c("_vbl_can_init", ["csrc/vbl/vbl_can.c"], ["VHAL_CAN"])
def _vbl_can_init():
    pass

_vbl_can_init()


class Can():
    """
================
The Can class
================

.. class:: Can(drvname, baud, options=0, sample_point=75.0, prop=0, phase1=0, phase2=0, sjw=1)

        This class implements CAN controller functionalities.

        Can is initialized by passing the driver name *drvname*, in the form of CAN0, CAN1, etc... depending on the
        board capabilities. Refer to the board pinout to locate the actual pins belonging to the driver.

        The bus speed must be also specified in the *baud* argument. Optional timing parameters can specify
        either the bit sample point *sample_point*, as a percentage of the bit time, or standard CAN parameters
        as propagation segment *prop*, phase segment 1 *phase1* and phase segment 2 *phase2*.

        When the sample point is used, the bit timing arguments must be zero and they are calculated automatically.
        The synchronization jump width *sjw* can be specified in any case.

        :param drvname: peripheral driver identifier (one of CAN0, CAN1, etc...)
        :param baud: bus speed, in bits per second (up to 1000000)
        :param sample_point: bit sample point, in percent (0 to 100.0)
        :param prop: propagation segment, in time quanta (1 to 8)
        :param phase1: phase segment 1, in time quanta (1 to 8)
        :param phase2: phase segment 2, in time quanta (1 to 8)
        :param sjw: synchronization jump width, in time quanta (1 to 4)
        :param options: a combination of the following option flags:

            * ``OPTION_NO_RETRY`` to prevent automatic retransmission (one-shot mode)
            * ``OPTION_NO_AUTO_OFF`` to prevent automatic recovery from Bus-Off errors
            * ``OPTION_KEEP_ORDER`` to maintain the chronological order of messages
            * ``OPTION_LISTEN_ONLY`` to silently listen to the bus (Ack is sent internally), receive-only mode, no transmission possible
            * ``OPTION_LOOPBACK`` to activate internal loopback test mode (can be combined with listen-only)

    """
    def __init__(self, drvname, baud, options=0, sample_point=75.0, prop=0, phase1=0, phase2=0, sjw=1):
        self.drv = __driver(drvname)
        self.drvid = drvname & 0xff
        if prop==0 or phase1==0 or phase2==0:
            (prop, phase1, phase2) = self.drv.__ctl__(_CANDRIVER_TIMING, self.drvid, baud, int(sample_point*10))
        self.drv.__ctl__(_CANDRIVER_INIT, self.drvid, baud, options, prop, phase1, phase2, sjw)

    def add_filter(self, id, mask):
        """
.. method:: add_filter(id,mask)

        Add a message acceptance filter for this CAN controller. A received message is accepted if:

            ``(received_id ^ id) & mask == 0``

        The returned value is a number that uniquely identifies the filter and can be used to remove the filter at a later time.

        :param id: contains the message identifier (11-bit for standard frames, 29-bit for extended frames), \
            plus the ID Extension (IDE) flag ``FRAME_EXT_FLAG`` and the Remote Transmission Request (RTR) flag ``FRAME_RTR_FLAG``.
        :param mask: is a bit mask where each bit is 1 if the received message id must match the corresponding bit in the message `id` \
            or 0 if the corresponding bit in the message `id` is ignored. To match a single id you can use ``FRAME_STD_MASK`` \
            for standard frames or ``FRAME_EXT_MASK`` combined with ``FRAME_EXT_FLAG`` for extended frames.

        :returns: the index of the filter added.

        """
        return self.drv.__ctl__(_CANDRIVER_ADDFILTER, self.drvid, id, mask)

    def del_filter(self, filter):
        """
.. method:: del_filter(filter)

        Remove a previously added message acceptance filter.

        :param filter: the index of the filter to remove.

        """
        self.drv.__ctl__(_CANDRIVER_DELFILTER, self.drvid, filter)

    def transmit(self, id, dlc, data=None, timeout=-1):
        """
.. method:: transmit(id, dlc, data=None, timeout=-1)

        Send a message to the CAN bus output mailboxes.

        :param id: contains the message identifier (11-bit for standard frames, 29-bit for extended frames), \
            plus the ID Extension (IDE) flag ``FRAME_EXT_FLAG`` and the Remote Transmission Request (RTR) flag ``FRAME_RTR_FLAG``.
        :param dlc: is the Data Length Code (DLC) used in a Data Frame or a Remote Frame. For Data Frames it is \
            the number of data bytes included in the message.
        :param data: is a byte array that contains the message data (up to 8 bytes for classic CAN), \
            not used for Remote Frames (RTR bit set).
        :param timeout: the maximum time to wait for queuing the message or -1 to wait indefinitely

        """
        self.drv.__ctl__(_CANDRIVER_TX, self.drvid, id, dlc, data, timeout)

    def receive(self, data=None, timeout=-1):
        """
.. method:: receive(data=None, timeout=-1)

        Receive a message from the CAN bus input mailboxes.

        If a *data* bytearray is provided it is used to store received data, otherwise a new buffer is created.

        :param data: a bytes or bytearray object to receive message data or *None*
        :param timeout: the maximum time to wait for a message or -1 to wait indefinitely

        :returns: a tuple of the form `(id,dlc,data)`, where:

            * `id` contains the message identifier (11-bit for standard frames, 29-bit for extended frames), \
                plus the ID Extension (IDE) flag ``FRAME_EXT_FLAG`` and the Remote Transmission Request (RTR) flag ``FRAME_RTR_FLAG``.
            * `dlc` is the Data Length Code (DLC) used in a Data Frame or a Remote Frame. For Data Frames it is \
                the number of data bytes included in the message.
            * `data` is a byte array that contains the message data (up to 8 bytes for classic CAN), \
                not used for Remote Frames (RTR bit set).

        """
        if data is None:
            data = bytearray(64)
        return self.drv.__ctl__(_CANDRIVER_RX, self.drvid, data, timeout)

    def get_errors(self):
        """
.. method:: get_errors()

        Get the accumulated error information.

        :returns: a tuple of the form `(flags,rx_count,tx_count)`, where:

            * `flags` contains the error flags, a combinations of:

                * ``ERROR_WARNING`` in Error-Active mode, when the error counter is over the warning threshold (usually >= 96)
                * ``ERROR_PASSIVE`` in Error-Passive mode (error counter >= 128)
                * ``ERROR_BUSOFF`` in Bus-Off mode (error counter > 255)
                * ``ERROR_STUFF`` if Bit-Stuffing errors have been detected (more than 5 bits at the same level)
                * ``ERROR_FORM`` if Form errors have been detected (Dominant bits detected during Recessive spaces)
                * ``ERROR_ACK`` if no Ack bit is detected for a transmitted message
                * ``ERROR_RECESSIVE`` if error detected when trasmitting Recessive bits
                * ``ERROR_DOMINANT`` if error detected when trasmitting Dominant bits
                * ``ERROR_CRC`` if CRC mismatch have been detected
                * ``ERROR_RX_OVERRUN`` if the receive queue is full when receiving a new message
                * ``ERROR_TX_ARBLOST`` if transmitted message lost abitration due to higher priority message on the bus
                * ``ERROR_TX_ERROR`` if some error has been detected during transmission

            * `rx_count` is the current value of the Receive error counter.
            * `tx_count` is the current value of the Transmit error counter.

        """
        return self.drv.__ctl__(_CANDRIVER_ERRORS, self.drvid)

    def abort_receive(self):
        """
.. method:: abort_receive()

        Abort any pending :any:`receive()` calls, that will raise a *TimeoutError*, and prevent further messages
        to be received, raising *ConnectionAbortedError*, until resumed.

        """
        self.drv.__ctl__(_CANDRIVER_STOPRX, self.drvid)

    def abort_transmit(self):
        """
.. method:: abort_transmit()

        Abort any pending :any:`transmit()` calls, that will raise a *TimeoutError*, and prevent further messages
        to be transmitted, raising *ConnectionAbortedError*, until resumed.

        """
        self.drv.__ctl__(_CANDRIVER_STOPTX, self.drvid)

    def resume_receive(self):
        """
.. method:: resume_receive()

        Resume receiving messages from the bus.

        """
        self.drv.__ctl__(_CANDRIVER_RESUMERX, self.drvid)

    def resume_transmit(self):
        """
.. method:: resume_transmit()

        Resume transmitting messages to the bus.

        """
        self.drv.__ctl__(_CANDRIVER_RESUMETX, self.drvid)

    def done(self):
        """
.. method:: done()

        Close this CAN driver instance and release used resources. Any pending function call or subsequent calls
        will raise an *IOError* exception.

        """
        self.drv.__ctl__(_CANDRIVER_DONE, self.drvid)

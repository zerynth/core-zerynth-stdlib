"""
.. module:: spisd

*****
SpiSD
*****

This module handles operations on Standard Capacity and High Capacity SD Cards (SDSC, SDHC) through Spi protocol.
The following operations are allowed:

    * read/write single block, with block address specified through SDSC or SDHC convention;
    * read/write multiple blocks, with block address specified through SDSC or SDHC convention;
    * generic read/write data, with block address specified through SDHC convention for both SDCS and SDHC cards;
    * read cid register.

Block size is set by default to 512 bytes.

Address conventions:

    * SDSC convention allows to select a block through its first byte location in byte address (example: for a block size of 512 bytes, second block will be addressed 0x200);
    * SDHC convention allows to select a block through its block address (example: 2nd block address (starting from 0th block) is simply 0x2)

    """

import spi

GO_IDLE_STATE     = 0x00      # resets the SD card
SEND_CSD          = 0x09      # sends card-specific data
SEND_IF_COND      = 0x08      #
SEND_CID          = 0x0A      # sends card identification
STOP_TRANSMISSION = 0x0C      # stops reading transmission
SEND_STATUS       = 0x0D      # sends status
READ_SINGLE_BLOCK = 0x11      # reads a block at byte address
READ_BLOCKS       = 0x12      # reads multiple blocks
WRITE_BLOCK       = 0x18      # writes a block at byte address
WRITE_BLOCKS      = 0x19      # writes multiple blocks
SEND_OP_COND      = 0x29      # starts card initialization
APP_CMD           = 0x37      # prefix for application command
READ_OCR          = 0x3A      # reads OCR
SET_BLOCKLEN      = 0x10      # changes r/w block size

SUPP_CHECK        = 0x1AA     # supply voltage 2.7-3.6 + check pattern
IDLE_RESP1        = 0x1
RESP1             = 0x0

debug = False

def print_d(*args):
    if debug:
        print(*args)

def _parse_arg(arg,n):
    if type(arg) == PBYTEARRAY:
        return arg
    res = bytearray(n)
    for i in range(n):
        res[n-i-1] = arg & 0xff
        arg = (arg >> 8)
    return res

def _to_arg(barray):
    res = 0
    for b in barray:
        res = ((res << 8) + b)
    return res

TOKEN = {
    'data_token': bytearray(1),
    'mw_data_token': bytearray(1),
    'stop_tran_token': bytearray(1)
}

TOKEN['data_token'][0] = 0xfe
TOKEN['mw_data_token'][0] = 0xfc
TOKEN['stop_tran_token'][0] = 0xfd

class SpiSD(spi.Spi):
    """
===========
SpiSD class
===========

.. class:: SpiSD(drvname, cs, clock=1000000)

        Initialize an SD card specifying its:

            * MCU SPI circuitry *drvname* (one of SPI0, SPI1, ... check pinmap for details);
            * chip select pin *cs*;
            * clock *clock*, default at 1MHz

        The instance attribute *hc* is set to 1 if the card is recognized as an SDHC, to 0 otherwise.

    """
    def __init__(self, drvname, cs, clock=1000000):

        spi.Spi.__init__(self, cs, drvname, clock=350000)

        self._clock = clock
        self._cs = cs
        self._drvname = drvname

        self._dummy_pkt = bytearray()
        self._dummy_pkt.append(0xff)

        self.block_len = 0x200

        self.hc = None

        self._init_sd()

    def _init_sd(self):

        self.unselect()
        self._start()

        print_d("WARM-UP START")
        self._send_dummy(20, select = False)
        print_d("WARM-UP END")

        print_d("SEND IDLE CMD")
        self._send_cmd(GO_IDLE_STATE,0,1,dummy_start=False,crc=0x95,cmp_resp=IDLE_RESP1)
        print_d("IDLE OK")
        res = self._send_cmd(SEND_IF_COND,SUPP_CHECK,5,crc=0x87)
        print_d("RESP VOLTAGE: ",hex(_to_arg(res[3:])))
        if _to_arg(res[3:]) != SUPP_CHECK:
            print_d("WRONG VOLTAGE")
            raise TypeError
        idle = True
        while idle:
            print_d("STILL IDLE")
            self._send_cmd(APP_CMD,0,1,cmp_resp=IDLE_RESP1)
            print_d("SEND OP COND")
            idle = (self._send_cmd(SEND_OP_COND,0x40000000,1)[0] == IDLE_RESP1)

        ocr = self._send_cmd(READ_OCR,0,5)
        for x in ocr:
            print_d("RESP OCR: ", x)
        if ((ocr[1] >> 6) & 1) == 0:
            # byte address, set block
            print_d("BLOCKLEN RES: ",self._send_cmd(SET_BLOCKLEN,self.block_len,1)[0])
            self.hc = 0
        else:
            print_d("HIGH CAPACITY SD")
            self.hc = 1
        self.clock = self._clock
        print_d("SD CARD INITED!")

    def _send_dummy(self,n, select = True):
        pkt = bytearray()
        for i in range(n):
            pkt.extend(self._dummy_pkt)
        if select:
            self.select()
        self.write(pkt)
        if select:
            self.unselect()

    def _addr_conv(self, addr):
        if self.hc == 0:
            return addr * self.block_len
        return addr

    def _wait_for(self, b, timeout = 500, start_val = None, not_eq = False):
        t_c = 0
        val = (self.exchange(self._dummy_pkt)[0] if start_val == None else start_val)
        cmp_res = ( val == b if not_eq else val != b )
        while cmp_res:
            val = self.exchange(self._dummy_pkt)[0]
            t_c += 1
            if t_c > timeout:
                raise TimeoutError
            cmp_res = ( val == b if not_eq else val != b )
        print_d("waited for ", t_c)
        return val

    def _wait_busy(self):
        self._wait_for(0x00, not_eq = True, timeout = 10000)

    def _send_cmd(self, cmd, arg, n, dummy_start = 8, unselect = True, crc=0x95, cmp_resp = None):
        if dummy_start:
            self._send_dummy(dummy_start)

        res = None
        pkt = bytearray()
        pkt.append(0x40 | cmd)
        pkt.extend(_parse_arg(arg,4))
        # pkt.append((crc7(pkt) << 2) | 1)
        pkt.append(crc)
        self.select()
        self.write(pkt)
        if n > 0:
            x = self._wait_for(0xff, not_eq = True)
            res = bytearray()
            res.append(x)
            if (n != 1):
                res.extend(self.read(n-1))
        if unselect:
            self.unselect()
        if cmp_resp != None:
            if res[0] != cmp_resp:
                print_d("RECV: ", res[0], " EXPECTING: ", cmp_resp)
                raise TypeError
        return res

    def _build_data_pkt(self, data, mw=False):
        res = bytearray()
        # token
        if mw:
            res.extend(TOKEN['mw_data_token'])
        else:
            res.extend(TOKEN['data_token'])
        res.extend(data)
        # 2 crc
        res.extend(self._dummy_pkt)
        res.extend(self._dummy_pkt)
        return res

    def _read_data_pkt(self, data_len = -1):
        if data_len == -1:
            data_len = self.block_len
        self._wait_for(TOKEN['data_token'][0])
        # read 512 byte block + 2 crc
        data = self.read(data_len)
        # crc
        self.write(self._dummy_pkt)
        self.write(self._dummy_pkt)
        return data

    def single_block_read(self, addr):
        """
.. method:: single_block_read(addr)

        Read a single block at address *addr*, following SDSC or SDHC address convention depending on used card.

        """
        self._send_cmd(READ_SINGLE_BLOCK, addr, 1, dummy_start = 20, unselect = False, cmp_resp = RESP1)
        data_block = self._read_data_pkt()
        self.unselect()

        return data_block

    def multiple_blocks_read(self, addr, n):
        """
.. method:: multiple_blocks_read(addr, n)

        Read n blocks starting from address *addr*, following SDSC or SDHC address convention depending on used card.

        """
        # # R1 - packet - packet - ... - CMD12 - stuff + wait - R1b
        # data = bytearray()
        # # print_d("SET M COUNT")
        # # self._send_cmd(APP_CMD,0,1, cmp_resp = RESP1)
        # # self._send_cmd(0x17, 2, 1, unselect = False, cmp_resp = RESP1)
        # print_d("READ BLOCKS")
        # self._send_cmd(READ_BLOCKS, addr, 1, dummy_start = 0, unselect = False, cmp_resp = RESP1)
        # print_d("R1 RECV")
        # for i in range(n):
        #     data.extend(self._read_data_pkt())
        # print_d("READ DATA PKTS")
        # self._send_cmd(STOP_TRANSMISSION | 0x40, 0, 0, dummy_start = 0, unselect = False)
        #
        # # discard stuff
        # self.write(self._dummy_pkt)
        #
        # print_d("WAITING FOR R1")
        # self.wait_for(RESP1)
        # print_d("BUSY")
        # self._wait_busy()
        #
        # print("DONE")
        # self.unselect()
        # return data

        data = bytearray()
        for i in range(n):
            data.extend(self.single_block_read(addr + self._addr_conv(i)))
        return data

    def read_data(self, addr, n):
        """
.. method:: read_data(addr, n)

        Read n blocks starting from address *addr*, SDHC address convention is used.

        """
        addr = self._addr_conv(addr)
        if n == 1:
            return self.single_block_read(addr)
        return self.multiple_blocks_read(addr, n)

    def _check_data_response(self, response):
        if (((response >> 1) & 0b111) != 0b010):
            raise TypeError

    def single_block_write(self, addr, data):
        """
.. method:: single_block_write(addr, data)

        Write a single block at address *addr*, following SDSC or SDHC address convention depending on used card.
        *data* must be a 512-byte long bytearray.

        """
        dummy_pkt = self._dummy_pkt
        pkt = self._build_data_pkt(data)
        self._send_cmd(WRITE_BLOCK, addr, 1, dummy_start = 20, unselect = False, cmp_resp = RESP1)

        self.write(dummy_pkt)
        self.write(pkt)

        self._check_data_response(self.read(1)[0])

        self._wait_busy()

        self.unselect()

    def multiple_blocks_write(self, addr, data):
        """
.. method:: multiple_blocks_write(addr, data)

        Write *data* starting from address *addr*, following SDSC or SDHC address convention depending on used card.

        Two formats allowed for *data*:

            * bytearray with a len multiple of 512 bytes.
            * list of 512-byte long bytearrays, where each bytearray inside data list contains data for a single block::

                block_1 = bytearray(0x200)
                block_2 = bytearray(0x200)
                ...
                [ block_1 , block_2 , ... ]

        """
        if type(data) == PBYTEARRAY:
            data_len = len(data)
            if (data_len % self.block_len) != 0:
                # MUST be multiple of block size
                raise TypeError
            blocks = (data_len//self.block_len)
            print_d("data_len: ", data_len, "BLOCKS: ", blocks)
            it = range(blocks)
        else:
            it = data

        dummy_pkt = self._dummy_pkt
        self._send_cmd(WRITE_BLOCKS, addr, 1, dummy_start = 20, unselect = False, cmp_resp = RESP1)

        self.write(dummy_pkt)


        for x in it:
            if type(data) == PBYTEARRAY:
                self.write(self._build_data_pkt(data[self.block_len*x:self.block_len*(x+1)],mw=True))
            else:
                self.write(self._build_data_pkt(x,mw=True))

            self._check_data_response(self.read(1)[0])
            self._wait_busy()

        self.write(TOKEN['stop_tran_token'])
        self.write(dummy_pkt)
        self._wait_busy()

        # send and unselect
        res = self._send_cmd(SEND_STATUS, 0, 2, dummy_start = 20)
        for x in res:
            print_d("STATUS: ", x)

    def write_data(self, addr, data):
        """
.. method:: write_data(addr, data)

        Write *data* starting from address *addr*, SDHC address convention is used.

        *data* format is defined as in :meth:`multiple_blocks_write` .

        """
        addr = self._addr_conv(addr)
        if (type(data) == PBYTEARRAY and len(data) == 512):
            self.single_block_write(addr, data)
        elif (type(data) == PLIST and len(data) == 1):
            self.single_block_write(addr, data[0])
        else:
            self.multiple_blocks_write(addr, data)

    def read_cid(self):
        """
.. method:: read_cid()

        Read 16-byte long cid register value.

        """
        self._send_cmd(SEND_CID, 0, 1, crc = 0x1b, dummy_start = 20, unselect = False, cmp_resp = RESP1)
        data_block = self._read_data_pkt(0x10)
        self.unselect()

        return data_block

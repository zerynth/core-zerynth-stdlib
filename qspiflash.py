
def _auto_init():
    if __defined(BOARD, "polaris_3g"):
        _init(D34,D33,D25,D24,D74,D75,0x100000,0x10000,0x8000,0x1000,0x100,8,4,4,2,4,0xA5,0x00,0x01,0x02,0x7C,0x80,0x02,0x80)
    else:
        raise UnsupportedError

@c_native("qspiflash_init", ["csrc/qspiflash/qspiflash.c"], ["VHAL_QSPIFLASH"])
def _init(d0,d1,d2,d3,clk,cs,flash_size,block_size,subblock_size,sector_size,page_size,dummy_cycles_read,dummy_cycles_read_dual,dummy_cycles_read_quad,dummy_cycles_2read,dummy_cycles_4read,alt_bytes_pe_mode,alt_bytes_no_pe_mode,sr_wip,sr_wel,sr_bp,sr_srwd,sr1_qe,sr1_sus):
    pass

@c_native("qspiflash_read_data",[],[])
def _read_data(addr,n=1):
    pass

@c_native("qspiflash_write_data",[],[])
def _write_data(addr, data):
    pass

@c_native("qspiflash_erase_block",[],[])
def _erase_block(addr):
    pass

@c_native("qspiflash_erase_sector",[],[])
def _erase_sector(addr):
    pass

@c_native("qspiflash_erase_chip",[],[])
def _erase_chip():
    pass

@c_native("qspiflash_sleep",[],[])
def _sleep():
    pass

@c_native("qspiflash_wakeup",[],[])
def _wakeup():
    pass

@c_native("qspiflash_done",[],[])
def _done():
    pass

class QSpiFlash():
    """
===============
QSpiFlash class
===============

.. class:: QSpiFlash()

        Initialize a a QspiFlash peripheral (external flash memory handled by qspi).
        This peripheral is available only for stm32l4 family chip and for Polaris Tracker device the auto_init is implemented (pins and memory data already configured). 

        To initialize a custom external memory qspi flash several params must be passed to the init method:

        :param d0: D0 pin of the Qspi peripheral
        :param d1: D1 pin of the Qspi peripheral
        :param d2: D2 pin of the Qspi peripheral
        :param d3: D3 pin of the Qspi peripheral
        :param clk: CLK pin of the Qspi peripheral
        :param cs: CS pin of the Qspi peripheral
        :param flash_size: Flash size of the qspi flash
        :param block_size: Block size of the qspi flash
        :param subblock_size: Sub-block size of the qspi flash
        :param sector_size Sector size of the qspi flash
        :param page_size: Page size of the qspi flash
        :param dummy_cycles_read: Dummy cycles simple read
        :param dummy_cycles_read_dual: Dummy cycles Dual flash read
        :param dummy_cycles_read_quad: Dummy cicles Quad Flash read
        :param dummy_cycles_2read: Dummy cycles 2read
        :param dummy_cycles_4read: Dummy Cycles 4read
        :param alt_bytes_pe_mode: Alternate Bytes for PE mode
        :param alt_bytes_no_pe_mode: Alternate Bytes for NO PE mode
        :param sr_wip: Write in progress of the flash mamory status register
        :param sr_wel: Write enable latch of the flash mamory status register
        :param sr_bp: Block protect of the flash mamory status register
        :param sr_srwd: Write disable of the flash mamory status register
        :param sr1_qe: Quad enable of the flash mamory status register1
        :param sr1_sus: Suspend status of the flash mamory status register1

    """
    def __init__(self, d0=None, d1=None, d2=None, d3=None, clk=None, cs=None, flash_size=None, block_size=None, subblock_size=None, sector_size=None, page_size=None, dummy_cycles_read=None, dummy_cycles_read_dual=None, dummy_cycles_read_quad=None, dummy_cycles_2read=None, dummy_cycles_4read=None, alt_bytes_pe_mode=None, alt_bytes_no_pe_mode=None, sr_wip=None, sr_wel=None, sr_bp=None, sr_srwd=None, sr1_qe=None, sr1_sus=None):
        if d0 is None:
            _auto_init()
        else:
            _init(d0,d1,d2,d3,clk,cs,flash_size,block_size,subblock_size,sector_size,page_size,dummy_cycles_read,dummy_cycles_read_dual,dummy_cycles_read_quad,dummy_cycles_2read,dummy_cycles_4read,alt_bytes_pe_mode,alt_bytes_no_pe_mode,sr_wip,sr_wel,sr_bp,sr_srwd,sr1_qe,sr1_sus)

    def __getitem__(self, key):
        return _read_data(key[0],key[1]-key[0])

    def __setitem__(self, addr, data):
        _write_data(addr, data)

    def write_data(self, addr, data):
        """
.. method:: write_data(addr, data)

        Write data *data* starting from address *addr*.
        *data* can be a bytearray or a list of integers less than 256.

        :meth:`erase_sector` MUST be called before writing data in a sector.

        Writing is also allowed via bracket notation. The following is valid syntax: ::

            my_flash[addr] = data

        """
        _write_data(addr, data)

    def erase_sector(self, addr):
        """
.. method:: erase_sector(addr)

        Erase a whole sector passing the *addr* address of any byte contained in it.
        All sector bytes set to 0xff.

        """
        _erase_sector(addr)

    def erase_block(self, addr):
        """
.. method:: erase_block(addr)

        Erase a memory block passing the *addr* address of any byte contained in it.
        All block bytes set to 0xff.

        """
        _erase_block(addr)

    def read_data(self, addr, n=1):
        """
.. method:: read_data(addr, n=1)

        Read *n* bytes of data starting from address *addr*.

        Reading is also allowed via bracket notation. The following is valid syntax: ::

            my_data = my_flash[addr:addr+n]

        """
        return _read_data(addr, n)

    def chip_erase(self):
        """
.. method:: chip_erase()

        Erase the whole memory.
        All memory bytes set to 0xff.

        """
        _erase_chip()

    def done(self):
        """
.. method:: done()

        Close the QspiFlash peripheral.

        """
        _done()

    def wakeup(self):
        """
.. method:: wakeup()

        Wake Up the QspiFlash peripheral from sleep mode.

        """
        _wakeup()

    def sleep(self):
        """
.. method:: sleep()

        Put the QspiFlash peripheral in sleep mode.

        """
        _sleep()

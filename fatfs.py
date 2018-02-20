"""
.. module:: fatfs

*****
fatFS
*****

This module implements Zerynth binding to `FatFS C library <http://elm-chan.org/fsw/ff/00index_e.html>`_ to handle FAT disks.

======
Driver
======

    """

import threading


# Volume Management

@native_c("__f_mount",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_mount(path):
    pass

@native_c("__update_disks_dict",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __update_disks_dict(pdrv, disk_args):
    pass

disks_dict = None

def mount(path, args):
    """
.. function:: mount(path, args)

    Register/unregister a file system object to the fatFs module.
    There is no activity of the physical drive in this function: physical mount process will be attempted during first file access.

    Required arguments are:

        * path string, following FatFS path convention::

            path = "0:/" # to mount your drive as volume 0

            generic_file_path = "0:/my/file/path"


        * args dictionary containing disk initialization parameters::

            # correct format for SD Card read through SPI protocol
            args = {"drv": SPI0, "cs": D25, "clock": 1000000}

            # correct format for SD Card read through SD mode
            # (be careful in choosing frequency (kHz) and bits supported by your board)
            args = {"drv": "SD0", "freq_khz": 20000, "bits": 1}
    """
    global disks_dict
    __builtins__.__default_fs = __module__
    pdrv = int(path.split(':')[0])
    disk_type = None
    if len(args) == 3:
        if 'cs' in args:
            disks_dict = __update_disks_dict(pdrv, [0, args["drv"], args["cs"], args["clock"]])
        else:
            disks_dict = __update_disks_dict(pdrv, [1, int(args["drv"][-1:]), args["bits"], args["freq_khz"]])
    else:
        raise ValueError
    __f_mount(path)

# File Access

@native_c("__f_open",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_open(path, mode, n):
    """
================================
File/Directory Access/Management
================================

File/Directory access/management is handled by os module, which needs a filesystem to be mounted
and a list of low-level functions implemented in the filesystem module.

    * File Access

        * __f_open
        * __f_close
        * __f_read
        * __f_write
        * __f_seek
        * __f_size
        * __f_tell
        * __f_truncate
        * __f_eof

    * Directory Access

        * __f_opendir
        * __f_closedir
        * __f_readdir

    * File/Directory Management

        * __f_copy
        * __f_unlink
        * __f_rename
        * __f_mkdir
        * __f_chdir
        * __f_getcwd
        * __f_exists
        * __f_isdir

    * Misc

        * get_available_fd_n
        * free_fd_n
        * to_b_mode

    """    
    pass

@native_c("__f_close",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_close(n):
    pass

@native_c("__f_read",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_read(n_bytes, mode, n):
    pass

@native_c("__f_write",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_write(to_w, sync, n):
    pass

@native_c("__f_seek",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_seek(pos, n):
    pass

@native_c("__f_size",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_size(n):
    pass

@native_c("__f_tell",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_tell(n):
    pass

@native_c("__f_truncate",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_truncate(n):
    pass

@native_c("__f_eof",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_eof(n):
    pass

# Directory Access

@native_c("__f_opendir",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_opendir(path, n):
    pass

@native_c("__f_closedir",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_closedir(n):
    pass

@native_c("__f_readdir",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_readdir(n):
    pass

# File/Directory Management

@native_c("__f_copy",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_copy(src, dst, n_src, n_dst):
    pass

@native_c("__f_unlink",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_unlink(path):
    pass

@native_c("__f_rename",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_rename(src_path, dst_path):
    pass

@native_c("__f_mkdir",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_mkdir(path):
    pass

@native_c("__f_chdir",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_chdir(path):
    pass

@native_c("__f_getcwd",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_getcwd(max_length):
    pass

@native_c("__f_exists",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_exists(path):
    pass

@native_c("__f_isdir",["csrc/fatfs/*"],["VHAL_SPI", "VHAL_SPISD", "VHAL_SDIO"])
def __f_isdir(path):
    pass


#### FileIO

_open_fd_lock = { 'files': threading.Lock(), 'dirs': threading.Lock() }
_open_fd = { 'files': [], 'dirs': [] }

def get_available_fd_n(fd):
    _open_fd_lock[fd].acquire()
    n = None
    for i in range(4):
        if not (i in _open_fd[fd]):
            n = i
            break
    if n == None:
        # no file available
        pass
    else:
        _open_fd[fd].append(n)
    _open_fd_lock[fd].release()
    return n

def free_fd_n(fd, n):
    _open_fd_lock[fd].acquire()
    _open_fd[fd].remove(n)
    _open_fd_lock[fd].release()

FA_READ			 = 0x01
FA_WRITE		 = 0x02
FA_OPEN_EXISTING = 0x00
FA_CREATE_NEW	 = 0x04
FA_CREATE_ALWAYS = 0x08
FA_OPEN_ALWAYS	 = 0x10

_str_modes = { 'r': FA_READ, 'w': FA_WRITE | FA_CREATE_ALWAYS, '+': FA_WRITE | FA_READ, 'a': FA_WRITE }

def to_b_mode(mode):
    res = 0
    for x in mode:
        res |= _str_modes[x]
    return res

####

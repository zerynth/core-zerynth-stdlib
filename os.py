"""
.. module:: os

****************
Operating System
****************

This module implements a generic os interface.
To function correctly it needs a filesystem to be mounted, so that the module can use
the driver to access the underlying hardware.

.. note:: Every path needed as argument for an os function must follow chosen driver path convention. File/Directory access error due to low-level failure will raise an OSError.

    """


# should be builtin

def open(path, mode):
    return FileIO(path, mode)

# should be os.path

path = __module__

def exists(path):
    """
.. function:: exists(path)

    Return True if path refers to an existing path, False otherwise.

    """
    return __default_fs.__f_exists(path)

def isdir(path):
    """
.. function:: isdir(path)

    Return True if path is an existing directory, False otherwise.

    """
    return __default_fs.__f_isdir(path)

#####

def remove(path):
    """
.. function:: remove(path)

    Remove (delete) the file path. If path is a directory, OSError is raised; see :func:`rmdir` below to remove a directory.

    """
    if isdir(path):
        raise OSError
    if __default_fs.__f_unlink(path) == -1:
        raise OSError

def rmdir(path):
    """
.. function:: rmdir(path)

    Remove (delete) the directory path. Only works when the directory is empty, otherwise, OSError is raised.

    """
    if not isdir(path):
        raise OSError
    if __default_fs.__f_unlink(path) == -1:
        raise OSError

def rename(src_path, dst_path):
    """
.. function:: rename(src_path, dst_path)

    Rename the file or directory src_path to dst_path. If dst_path is a directory, OSError will be raised.

    """
    if isdir(dst_path):
        raise OSError
    if __default_fs.__f_rename(src_path, dst_path) == -1:
        raise OSError

def mkdir(path):
    """
.. function:: mkdir(path)

    Create a directory named path.

    """
    if __default_fs.__f_mkdir(path) == -1:
        raise OSError

def chdir(path):
    """
.. function:: chdir(path)

    Change the current working directory to path.

    """
    if __default_fs.__f_chdir(path) == -1:
        raise OSError

cwd_max_length = 128

def getcwd():
    """
.. function:: getcwd()

    Return a string representing the current working directory.

    Since C filesystem drivers need a limit on returned string length::

        os.cwd_max_length

    attribute is set to 128 by default but can be adjusted, if needed, before calling :meth:`getcwd`.
    """
    res = __default_fs.__f_getcwd(cwd_max_length)
    if res == -1:
        raise OSError
    return res

def listdir(path):
    """
.. function:: listdir(path)

    Return a list containing the names of the entries in the directory given by path. The list is in arbitrary order.
    It does not include the special entries '.' and '..' even if they are present in the directory.

    """
    n = __default_fs.get_available_fd_n('dirs')
    if __default_fs.__f_opendir(path, n) == -1:
        __default_fs.free_fd_n('dirs')
        raise OSError
    dir_list = []
    while True:
        res = __default_fs.__f_readdir(n)
        if res == -1:
            raise OSError
        elif res == 0:
            break
        else:
            dir_list.append(res)
    __default_fs.__f_closedir(n)
    __default_fs.free_fd_n('dirs', n)
    return dir_list

# should be shutil

def copyfile(src, dst):
    """
.. function:: copyfile(src, dst)

    Copy the contents of the file named src to a file named dst.

    """
    n_src = __default_fs.get_available_fd_n('files')
    n_dst = __default_fs.get_available_fd_n('files')
    res = __default_fs.__f_copy(src, dst, n_src, n_dst)
    __default_fs.free_fd_n('files', n_src)
    __default_fs.free_fd_n('files', n_dst)
    if res == -1:
        raise OSError

# should be io

class FileIO():
    """
================
The FileIO class
================

.. class:: FileIO(path, mode = 'r')

    Main class to handle files.

        * path is a string (following chosen low-level filesystem driver format)
        * mode is a string:

            * 'r' 	open for reading (default)
            * 'w' 	open for writing, truncating the file first
            * 'a' 	open for writing, appending to the end of the file if it exists
            * 'b' 	binary mode
            * '+' 	open a disk file for updating (reading and writing)
    """
    def __init__(self, path, mode = 'r'):
        self.closed = False
        self._read_mode = 0
        self._n = __default_fs.get_available_fd_n('files')
        do_seek = False

        if 'b' in mode:
            self._read_mode = 1
        mode = mode.replace('b','')
        if 'a' in mode:
            if not exists(path):
                mode = mode.replace('a','w')
            else:
                do_seek = True
        mode = __default_fs.to_b_mode(mode)

        r = __default_fs.__f_open(path, mode, self._n)
        if r == -1:
            raise OSError
        if do_seek:
            self.seek(0, 2)

    def read(self, n_bytes = -1):
        """
.. method:: read(n_bytes = -1)

        Read up to size bytes from the object and return them. As a convenience, if size is unspecified or -1, the whole file is read.
        Return type can be string or bytes depending on chosen mode.

        An empty string or an empty bytes object indicate end of file.

        """
        if self.closed:
            raise ValueError
        if n_bytes == -1:
            n_bytes = self.size()
        return __default_fs.__f_read(n_bytes, self._read_mode, self._n)

    def write(self, to_w, sync = False):
        """
.. method:: write(to_w, sync = False)

        Write to_w object (string or bytes) to the stream and return the number of characters written.

        *sync* parameter allows to write changes to disk immediately, without waiting :meth:`close` call.
        """
        if self.closed:
            raise ValueError
        return __default_fs.__f_write(to_w, sync, self._n)

    def close(self):
        """
.. method:: close()

        Close file stream.
        """
        if self.closed:
            raise ValueError
        __default_fs.free_fd_n('files', self._n)
        r = __default_fs.__f_close(self._n)
        if r == -1:
            raise OSError
        self.closed = True

    def size(self):
        """
.. method:: size()

        Return file size.
        """
        if self.closed:
            raise ValueError
        return __default_fs.__f_size(self._n)

    def tell(self):
        """
.. method:: tell()

        Return the current stream position.
        """
        if self.closed:
            raise ValueError
        return __default_fs.__f_tell(self._n)

    def seek(self, offset, whence = 0):
        """
.. method:: seek(offset, whence=0)

        Move the current position to *offset* bytes with respect to *whence*.

        *whence* can be:

            * 0: start of file
            * 1: current position
            * 2: end of file

        """
        if self.closed:
            raise ValueError
        if whence == 1:
            whence = self.tell()
        elif whence == 2:
            whence = self.size()
        if __default_fs.__f_seek(whence + offset, self._n) == -1:
            return -1
        return self.tell()

    def truncate(self, size = None):
        """
.. method:: truncate()

        Resize the stream to the given size in bytes (or the current position if size is not specified).
        The current stream position isn’t changed.
        """
        if self.closed:
            raise ValueError
        if size != None:
            self.seek(size)
        if __default_fs.__f_truncate(self._n) == -1:
            raise OSError

    def readline(self):
        """
.. method:: readline()

        Read until newline or EOF and return a single str.
        If the stream is already at EOF, an empty string is returned.
        """
        if self.closed or self._read_mode == 'b':
            raise ValueError
        res = ""
        n_chr = None
        while n_chr != '\n' and n_chr != '':
            n_chr = self.read(1)
            res += n_chr
        return res

    def __eof(self):
        if self.closed:
            raise ValueError
        return __default_fs.__f_eof(self._n)

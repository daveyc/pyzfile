import ctypes
from ctypes import *
import pathlib
import json


class ZFile:
    """
    A class for record oriented I/O on MVS data sets.
    """

    def __init__(self, filename: str, mode: str, encoding: str = None):
        # load the shared lib
        lib_file = pathlib.Path(__file__).parent / "libzfile.so"
        self.lib = ctypes.CDLL(str(lib_file))
        self.handle = None
        self.filename = filename
        self.mode = mode
        self.encoding = encoding

        self.BUFFER_SIZE = 1024 * 64  # maximum length of an MVS data set record
        self.buffer = ctypes.create_string_buffer(1024 * 64)

        # set the function prototypes
        self.lib.zfile_open.argtypes = [c_char_p, c_char_p]
        self.lib.zfile_open.restype = c_void_p

        self.lib.zfile_close.argtypes = [c_void_p]
        self.lib.zfile_close.restype = c_int

        self.lib.zfile_read.argtypes = [c_void_p, c_size_t, c_size_t, c_void_p]
        self.lib.zfile_read.restype = c_size_t

        self.lib.zfile_write.argtypes = [c_void_p, c_size_t, c_size_t, c_void_p]
        self.lib.zfile_write.restype = c_size_t

        self.lib.zfile_locate.argtypes = [c_void_p, c_void_p, c_size_t, c_char_p]
        self.lib.zfile_locate.restype = c_int32

        self.lib.zfile_update.argtypes = [c_void_p, c_void_p, c_size_t]
        self.lib.zfile_update.restype = c_int32

        self.lib.zfile_delrec.argtypes = [c_void_p]
        self.lib.zfile_delrec.restype = c_int

        self.lib.zfile_info.argtypes = [c_void_p, c_char_p, c_size_t]
        self.lib.zfile_info.restype = c_size_t

        self.lib.zfile_amrc.argtypes = [c_void_p, c_char_p, c_size_t]
        self.lib.zfile_amrc.restype = c_size_t

        self.lib.zfile_rba.argtypes = [c_void_p]
        self.lib.zfile_rba.restype = c_int32

        self.lib.zfile_strerror.restype = c_char_p

    ####################################################################################################################
    # Context manager
    ####################################################################################################################
    def __enter__(self):
        return self.open()

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.close()

    ####################################################################################################################
    # Iterators
    ####################################################################################################################
    def __iter__(self):
        return self

    def __next__(self):
        ret = self.read()
        if not ret:
            raise StopIteration
        return ret

    ####################################################################################################################
    # Wrapper functions
    ####################################################################################################################
    def open(self):
        self.handle = self.lib.zfile_open(c_char_p(self.filename.encode("ascii")), c_char_p(self.mode.encode("ascii")))
        if not self.handle:
            raise Exception(f"Error opening file '{self.filename}': {self.lib.zfile_strerror().decode('utf-8')}")
        return self

    def close(self):
        if self.handle:
            self.lib.zfile_close(self.handle)
            self.handle = None

    def read(self, length: int = None) -> None | str | bytes:
        """
        Reads a record from the file.

        :param length: The number of bytes to read. If this argument is not specified then the length is set to the
            maximum buffer size of 65536.
        :return: If encoding has been specified then a UTF-8 string is returned, otherwise a bytes record.
            If end-of-file has been reached None is returned.
        """
        if not length:
            length = self.BUFFER_SIZE
        bytes_read = self.lib.zfile_read(self.buffer, 1, length, self.handle)
        if bytes_read == 0:
            return None
        buffer = self.buffer.raw[:bytes_read]
        return buffer.decode(self.encoding) if self.encoding else buffer

    def write(self, rec, length: int = None):
        """
        Writes a record to the file.

        :param rec: The number of bytes to write. If ``encoding`` has been specified the record is encoded using the
            codeset.
        :param length: The number of bytes to write. If this argument is not specified then the length is set to the
            length of ``rec``.
        :raise ZFileError if an error occurs writing the record.
        """
        if self.encoding:
            rec = rec.encode(self.encoding)
        if not length:
            length = len(rec)
        ret = self.lib.zfile_write(rec, 1, length, self.handle)
        if ret == 0:
            raise Exception(f"Error writing to file '{self.filename}': {self.lib.zfile_strerror().decode('utf-8')}")

    def locate(self, rec, option: str, length: int = None):
        """
        Locates a record in a VSAM file.

        :param rec: The record to locate
        :param option: An option from the following list: ``key_first``, ``key_last``, ``key_eq``, ``key_eq_bwd``,
            ``key_ge``, ``rba_eq``, ``rba_eq_bwd``
        :param length: The length of the record. If this argument is not specified the length of record is used.
        :return: True if the record was located, otherwise False.
        """
        if self.encoding:
            rec = rec.encode(self.encoding)
        if not length:
            length = len(rec)
        ret = self.lib.zfile_locate(self.handle, rec, length, option.encode("ascii"))
        # TODO: throw exception on argument error
        return True if ret == 0 else False

    def update(self, rec, length: int = None):
        """
        Replaces the last record read from a VSAM file with the contents of rec.

        :param rec: The record to replace.
        :param length: The length of the record. If this argument is not specified then the length of ``rec`` is used
            implicitly.
        :return: True if the record was updated, otherwise False.
        """
        if self.encoding:
            rec = rec.encode(self.encoding)
        if not length:
            length = len(rec)
        ret = self.lib.zfile_write(self.handle, rec, length)
        return True if ret == 0 else False

    def info(self):
        """
        Returns a dict with information about the file. See the z/OS C/C++ runtime reference for the ``fldata()```
        function and the ``fldata_t`` structure.

        ::

            {'access_method': 'UNSPEC',
             'blksize': 32756,
             'device': 'DISK',
             'dsname': 'TS8004.TXC.REGISTER',
             'dsorgConcat': False,
             'dsorgHFS': False,
             'dsorgHiper': False,
             'dsorgMem': False,
             'dsorgPDE': False,
             'dsorgPDSdir': False,
             'dsorgPDSmem': False,
             'dsorgPO': False,
             'dsorgPS': False,
             'dsorgTemp': False,
             'dsorgVSAM': True,
             'maxreclen': 32756,
             'mode': {'append': False, 'read': True, 'update': False, 'write': False},
             'noseek_to_seek': 'NOSWITCH',
             'openmode': 'RECORD',
             'recfmASA': False,
             'recfmB': False,
             'recfmBlk': False,
             'recfmF': False,
             'recfmM': False,
             'recfmS': False,
             'recfmU': True,
             'recfmV': False,
             'vsamRKP': 0,
             'vsamRLS': 'NORLS',
             'vsamkeylen': 64,
             'vsamtype': 'KSDS'}

        :return: A dict containing information from the ``fldata_t`` structure.
        """
        size = self.lib.zfile_info(self.handle, self.buffer, self.BUFFER_SIZE)
        return json.loads(self._buf2str(self.buffer, size))

    def amrc(self):
        """
        Returns the ``__amrc`` structure defined in stdio.h to help you determine errors resulting from
        an I/O operation.         See the z/OS C/C++ programming guide for documentation on the content
        of this stucture.

        ::

            {'amrc_noseek_to_seek': 'NOSWITCH',
             'code': {
                 'abend': {'rc': 0, 'syscode': 0},
                 'alloc': {'svc_error': 0, 'svc_info': 0},
                 'error': 0,
                 'feedback': {'fdbk': 0, 'rc': 0, 'rtncd': 0}
              },
             'last_op': 'VSAM_OPEN_KSDS',
             'msg': {'len': 0, 'parmr0': 0, 'parmr1': 0, 'str': '', 'xrba': 0},
             'rba': 0
            }

        :return: a dict containing fields from the ``__amrc`` structure.
        """
        size = self.lib.zfile_amrc(self.handle, self.buffer, self.BUFFER_SIZE)
        return json.loads(self._buf2str(self.buffer, size))

    def rba(self):
        return self.lib.zfile_rba(self.handle)

    @staticmethod
    def _buf2str(buffer: Array[c_char], size: int) -> str:
        return bytes(buffer.raw[:size]).decode("utf-8")


def open_zfile(filename: str, mode: str, encoding: str = None) -> ZFile:
    """
    Creates and opens a ZFile

    :param filename: The name of the file to open
    :param mode: The open mode. For example, r, rb, w, wb
    :param encoding: The file encoding
    :return: A new ZFile
    """
    return ZFile(filename, mode, encoding).open()

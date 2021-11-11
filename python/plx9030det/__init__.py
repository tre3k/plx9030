from ctypes import c_int, c_uint, c_void_p, cdll, c_char_p


class plx9030det(object):

    _lib = cdll.LoadLibrary('libplx9030detector.so')

    def __init__(self, chrdev):
        self._lib.plx9030det.argtypes = [c_char_p]
        self._lib.plx9030det.restype = c_void_p

        self._lib.getStatus.argtypes = [c_void_p]
        self._lib.getStatus.restype = c_uint

        self._lib.checkMem.argtypes = [c_void_p]
        self._lib.checkMem.restype = c_int

        self.obj = self._lib.plx9030det(chrdev)

    def getStatus(self):
        return self._lib.getStatus(self.obj)

    def checkMem(self):
        return self._lib.checkMem(self.obj)

    def test(self):
        return self._lib.test()

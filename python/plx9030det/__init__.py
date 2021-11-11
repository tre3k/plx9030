from ctypes import c_int, c_uint, c_void_p, cdll

lib = cdll.LoadLibrary('libplx9030detector.so')


class plx9030det(object):
    def __init__(self, chrdev):
        # lib.plx9030det.argtypes = [c_char_p]
        lib.plx9030det.restype = c_void_p

        lib.getStatus.argtypes = [c_void_p]
        lib.getStatus.restype = c_int

        lib.checkMem.argtypes = [c_void_p]
        lib.checkMem.restype = c_uint

        self.obj = lib.plx9030det(chrdev)

    def getStatus(self):
        return lib.getStatus(self.obj)

    def checkMem(self):
        return lib.checkMem(self.obj)

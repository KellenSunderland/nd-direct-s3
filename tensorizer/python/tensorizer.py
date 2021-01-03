import ctypes
import logging

from functools import lru_cache

class Tensorizer:

    def __init__(self):
        self.tensorizer = self._load_native_tensorizer()
        self.tensorizer_handle = self.tensorizer.construct()


    @lru_cache(maxsize=None)
    def _load_native_tensorizer(self):
        logging.info("Loading Tensorizer native lib")
        return ctypes.cdll.LoadLibrary("libtensorizer.so")


    def queue_ndarray(self, s3_bucket, s3_object):
        self.tensorizer.queue_ndarray(self.tensorizer_handle, s3_bucket, s3_object)


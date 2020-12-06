import ctypes
import logging

from functools import lru_cache

logging.basicConfig(level=logging.DEBUG)

@lru_cache(maxsize=None)
def _load_native_tensorizer():
    logging.info("Loading Tensorizer native lib")
    return ctypes.cdll.LoadLibrary("libtensorizer.so")

tensorizer = _load_native_tensorizer()
tensorizer_handle = tensorizer.construct()

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

#! /usr/bin/python3

import ctypes
from ctypes import POINTER, string_at, c_char_p, c_int, c_void_p, pointer, byref
import unittest
#
from common import PathToObjects, get_linenumber, get_function_name

paths = PathToObjects()
c_lib = ctypes.CDLL(paths.logger_library_location)


class LibStdOutTesting(unittest.TestCase):
    def setUp(self):
        c_lib.micro_logger_get_stdout_writer.restype = ctypes.c_void_p
        writer = c_lib.micro_logger_get_stdout_writer()
        c_lib.micro_logger_set_writer.argtypes = [c_void_p]
        c_lib.micro_logger_set_writer(writer)

        self.lvl_info = string_at(c_char_p.in_dll(c_lib, "LVL_INFO"))
        self.lvl_warn = string_at(c_char_p.in_dll(c_lib, "LVL_WARN"))
        self.lvl_error = string_at(c_char_p.in_dll(c_lib, "LVL_ERROR"))
        self.lvl_critical = string_at(c_char_p.in_dll(c_lib, "LVL_CRITICAL"))

    def test_msg_send_something(self):
        c_lib.micro_logger_logme(self.lvl_info,
                                 c_char_p(b'file'),
                                 c_char_p(get_function_name().encode('utf-8')),
                                 c_int(get_linenumber()),
                                 c_char_p(b'python is %s'),
                                 c_char_p(b'awesome')
                                 )


if __name__ == '__main__':
    unittest.main()

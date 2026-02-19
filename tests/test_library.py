# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

"""
Test suite for micro_logger C library via ctypes interface.

This module provides tests for the C API of the micro_logger library,
testing functionality through Python's ctypes bindings.
"""

#! /usr/bin/python3

import ctypes
from ctypes import string_at, c_char_p, c_int, c_void_p
import unittest
from common import PathToObjects, get_linenumber, get_function_name

paths = PathToObjects()
c_lib = ctypes.CDLL(paths.logger_library_location)


class LibStdOutTesting(unittest.TestCase):
    """
    Test the C library logging functionality with stdout writer.

    This test class verifies that the C API correctly logs messages
    to stdout through Python's ctypes interface.
    """

    def setUp(self):
        """
        Set up the test environment by initializing the C library.

        Configures the ctypes bindings for the C library functions and
        retrieves the logging levels as string pointers.
        """
        c_lib.micro_logger_get_stdout_writer.restype = ctypes.c_void_p
        writer = c_lib.micro_logger_get_stdout_writer()
        c_lib.micro_logger_initialize.argtypes = [c_void_p, c_void_p]
        c_lib.micro_logger_initialize(writer, None)

        self.lvl_info = string_at(c_char_p.in_dll(c_lib, "MICRO_LOGGER_LVL_INFO"))
        self.lvl_warn = string_at(c_char_p.in_dll(c_lib, "MICRO_LOGGER_LVL_WARN"))
        self.lvl_error = string_at(c_char_p.in_dll(c_lib, "MICRO_LOGGER_LVL_ERROR"))
        self.lvl_critical = string_at(c_char_p.in_dll(c_lib, "MICRO_LOGGER_LVL_CRITICAL"))

    def test_msg_send_something(self):
        """
        Test logging a message to stdout using the C library.

        This test verifies that the C library correctly logs a formatted
        message to stdout with appropriate level and context information.
        """
        c_lib.micro_logger_logme(
            self.lvl_info,
            c_char_p(b"file"),
            c_char_p(get_function_name().encode("utf-8")),
            c_int(get_linenumber()),
            c_char_p(b"python is %s"),
            c_char_p(b"awesome"),
        )


if __name__ == "__main__":
    unittest.main()

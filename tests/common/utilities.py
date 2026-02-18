# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

"""
Utility functions for test debugging and introspection.

This module provides helper functions for retrieving function names and line
numbers for use in test assertions and debugging.
"""

from inspect import currentframe, getouterframes


def get_linenumber():
    """
    Get the current line number in the source code.

    Returns:
        int: The line number of the calling function
    """
    return currentframe().f_back.f_lineno


def get_function_name():
    """
    Get the name of the current function.

    Returns:
        str: The name of the calling function
    """
    return getouterframes(currentframe())[1].function


def get_cpu_name():
    with open("/proc/cpuinfo") as f:
        for line in f:
            if "model name" in line:
                return (line.strip().split(":")[1])


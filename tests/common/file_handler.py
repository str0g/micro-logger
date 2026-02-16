# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

"""
Utility functions for handling test file operations.

This module provides functions for creating temporary files and parsing
log files for testing purposes.
"""

import os
import tempfile
from time import sleep

from .data_handler import DataHandler
from .custom_popen import match_line_data


def read_and_parse_file(file_path, msg_regex):
    """
    Read and parse log file using regex pattern.

    Args:
        file_path: Path to the log file
        msg_regex: Compiled regex pattern for matching log lines

    Returns:
        List of DataHandler objects parsed from the file
    """
    sleep(0.1)
    out = []
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"File not found: {file_path}")

    with open(file_path, "r+", encoding="utf-8") as f:
        for line in f.readlines():
            out.append(DataHandler(match_line_data(line, msg_regex)))
    return out


def create_temp_file(suffix=".txt"):
    """
    Create a temporary file for testing with automatic cleanup.

    Args:
        suffix: File extension for the temporary file

    Returns:
        Path to the created temporary file
    """
    fd, path = tempfile.mkstemp(suffix=suffix)
    os.close(fd)
    return path


# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

"""
Custom process execution utilities for testing.

This module provides utilities for running subprocesses, parsing output,
and handling environment variables for test execution.
"""

from subprocess import Popen, PIPE
import os
from .data_handler import DataHandler


def match_line_data(line, msg_regex):
    """
    Match a line of text against a regex pattern.

    Args:
        line: The line of text to match
        msg_regex: Compiled regex pattern for matching

    Returns:
        re.Match object if the pattern matches, None otherwise

    Raises:
        ValueError: If the pattern does not match the input line
    """
    match = msg_regex.match(line)
    if not match:
        raise ValueError("patter not matched")
    return match


def custom_popen(cmd, env):
    """
    Execute a command as a subprocess.

    Args:
        cmd: Command to execute as a list
        env: Environment variables to use for the subprocess

    Returns:
        Popen object for the running process
    """
    print(cmd)
    return Popen(cmd, stdout=PIPE, stderr=PIPE, env=env)


def custom_popen_wrapper(cmd, env, msg_regex, exp):
    """
    Execute a command and parse its output.

    Runs a subprocess, captures stdout/stderr, and parses the output
    using the provided regex pattern. Sets the expected test object's PID.

    Args:
        cmd: Command to execute as a list
        env: Environment variables to use
        msg_regex: Compiled regex pattern for matching output
        exp: Test object to set PID and validate output against

    Returns:
        List of DataHandler objects parsed from the output
    """
    with custom_popen(cmd, env) as process:
        exp.expected.pid = f"{process.pid:08d}"
        stdout = process.stdout.read().decode("utf-8")
        print(f"STDOUT -> |{stdout}|")
        out = []
        for line in stdout.split("\n")[:-1]:
            out.append(DataHandler(match_line_data(line, msg_regex)))
        return out


def get_envrion_variables(binary):
    """
    Get environment variables with ASan preload for memory debugging.

    Checks if the binary requires AddressSanitizer (ASan) to be preloaded
    and adds the appropriate preload library if needed.

    Args:
        binary: Path to the binary to check

    Returns:
        dict: Environment variables for the subprocess
    """
    env = os.environ.copy()
    with custom_popen([binary, "--help"], env) as proc:
        if str(proc.stderr.read()).__contains__(
            "ASan runtime does not come first in initial library list; you should either link runtime to your application or manually preload it with LD_PRELOAD"
        ):
            with custom_popen(["gcc", "-print-file-name=libasan.so"], env) as ld_pre:
                env["LD_PRELOAD"] = (
                    ld_pre.stdout.read().decode("utf-8").removesuffix("\n")
                )
    return env


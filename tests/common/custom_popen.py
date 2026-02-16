# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

from subprocess import Popen, PIPE
import os

from .data_handler import DataHandler


def match_line_data(line, msg_regex):
    match = msg_regex.match(line)
    if not match:
        raise ValueError("patter not matched")
    return match


def custom_popen(cmd, env):
    print(cmd)
    return Popen(cmd, stdout=PIPE, stderr=PIPE, env=env)


def custom_popen_wrapper(cmd, env, msg_regex, exp):
    with custom_popen(cmd, env) as process:
        exp.expected.pid = f"{process.pid:08d}"
        stdout = process.stdout.read().decode("utf-8")
        print(f"STDOUT -> |{stdout}|")
        out = []
        for line in stdout.split("\n")[:-1]:
            out.append(DataHandler(match_line_data(line, msg_regex)))
        return out


def get_envrion_variables(binary):
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


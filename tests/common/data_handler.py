# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

import re

regex_line_pattern = re.compile(
    r"""
\[(?P<date>\d{2}/\d{2}/\d{2})[ ](?P<time>\d{2}:\d{2}:\d{2}\.\d{3})\]
\[(?P<level>(TRACE|DEBUG|INFO[ ]|WARN[ ]|ERROR|CRITI))\]
\[pid:(?P<pid>\d{8})\]\[tid:(?P<tid>\d{16})\]
\[(?P<file>[a-zA-Z0-9_-]+\.[cp]{1,3}):(?P<line>\d{1,4})::(?P<function>.*?)\]\[(?P<message>.*?)\]
""",
    re.VERBOSE,
)


class DataHandler:
    def __init__(self, match=None) -> None:
        self.data = None
        self.time = None
        self.level = None
        self.pid = None
        self.tid = None
        self.file = None
        self.line = None
        self.function = None
        self.message = ""
        self.options = [[], ""]
        if match:
            self.data = match["date"]
            self.time = match["time"]
            self.level = match["level"]
            self.pid = match["pid"]
            self.tid = match["tid"]
            self.file = match["file"]
            self.line = match["line"]
            self.function = match["function"]
            self.message = match["message"]

    def __eq__(self, right) -> bool:
        if not isinstance(right, DataHandler):
            return NotImplemented

        return (
            self.level == right.level
            and (self.file == right.file)
            and (self.line == right.line)
            and (self.function == right.function)
            and (self.message == right.message)
        )

    def __hash__(self) -> int:
        return hash((self.level, self.file, self.line, self.function, self.message))

    def __str__(self) -> str:
        return f"""data: {self.data}
time: {self.time}
level: {self.level}
pid: {self.pid}
tid: {self.tid}
file: {self.file}
line: {self.line}
function: {self.function}
message: {self.message}"""

    def __repr__(self) -> str:
        return f"""data: level: {self.level}
pid: {self.pid}
file: {self.file}
line: {self.line}
function: {self.function}
message: {self.message}"""

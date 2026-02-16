# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

regex_benchmark_pattern = r"""(single|multi)thread[ ]took:[ ](?P<time>[0-9]+)ms,[ ]size:[ ](?P<size>[0-9]+),[ ]bandwidth:[ ](?P<bandwidth>[0-9.]+)[ ]MB/s"""


class BenchmarkValue:
    def __init__(self, match=None) -> None:
        self.time = None
        self.size = None
        self.bandwidth = None
        if match:
            self.time = int(match["time"])
            self.size = int(match["size"])
            self.bandwidth = float(match["bandwidth"])

    def __str__(self):
        return f"time: {self.time}\nsize: {self.size}\nbandwidth: {self.bandwidth}"

    def __eq__(self, right) -> bool:
        if not isinstance(right, BenchmarkValue):
            return NotImplemented

        return (
            self.delta(self.time, right.time, 0.11)
            and self.delta(self.size, right.size, 0)
            and self.delta(self.bandwidth, right.bandwidth, 0.11)
        )

    @staticmethod
    def delta(left, right, delta):
        if left == right:
            return True
        if left == 0:
            return right < delta

        val = abs(left - right)
        return val / left < delta


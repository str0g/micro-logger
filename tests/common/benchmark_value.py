# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

"""
Benchmark value parsing and comparison utilities.

This module provides classes and functions for parsing benchmark results
and comparing performance metrics with tolerance thresholds.
"""

regex_benchmark_pattern = r"""(single|multi)thread[ ]took:[ ](?P<time>[0-9]+)ms,[ ]size:[ ](?P<size>[0-9]+),[ ]bandwidth:[ ](?P<bandwidth>[0-9.]+)[ ]MB/s"""


class BenchmarkValue:
    """
    Represents a benchmark measurement with time, size, and bandwidth.

    This class parses benchmark output and provides comparison utilities
    for validating performance results within acceptable tolerance ranges.
    """

    def __init__(self, match=None) -> None:
        """Initialize with parsed match data or empty values."""
        self.time = None
        self.size = None
        self.bandwidth = None
        if match:
            self.time = int(match["time"])
            self.size = int(match["size"])
            self.bandwidth = float(match["bandwidth"])

    def __str__(self):
        """Return string representation of benchmark values."""
        return f"time: {self.time}\nsize: {self.size}\nbandwidth: {self.bandwidth}"

    def __repr__(self):
        return self.__str__()

    def __eq__(self, right) -> bool:
        """
        Compare two BenchmarkValue objects.

        Compares time, size, and bandwidth values with tolerance
        thresholds (11% for time and bandwidth, 0% for size).

        Args:
            right: Another BenchmarkValue to compare against

        Returns:
            bool: True if values are within tolerance, False otherwise
        """
        if not isinstance(right, BenchmarkValue):
            return NotImplemented

        return (
            self.delta(self.time, right.time, 0.11)
            and self.delta(self.size, right.size, 0)
            and self.delta(self.bandwidth, right.bandwidth, 0.11)
        )

    @staticmethod
    def delta(left, right, delta):
        """
        Compare two values with tolerance.

        Args:
            left: First value to compare
            right: Second value to compare
            delta: Maximum allowed relative difference (0 to 1)

        Returns:
            bool: True if values are within delta, False otherwise
        """
        if left == right:
            return True
        if left == 0:
            return right < delta

        val = abs(left - right)
        return val / left < delta


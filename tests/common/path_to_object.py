# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

"""
Path utilities for locating test resources.

This module provides utilities for finding project paths, library files,
and executables within the micro_logger project structure.
"""

from subprocess import Popen, PIPE
import pathlib


class PathToObjects:
    """
    Helper class for locating micro_logger project resources.

    This class finds the top-level project directory and locates the
    compiled library files and demo executables for testing.
    """

    def __init__(self):
        """Initialize by searching for the micro_logger project root."""
        project_name = ["micro-logger", "micro-logger.git"]
        library_name = "libmicro_logger"
        self.top_path = None
        obj = pathlib.Path.cwd()
        while obj.name:
            print(obj.name)
            if obj.name in project_name:
                self.top_path = obj
                break
            obj = obj.parent
        if not self.top_path:
            raise ValueError(f"could not find project {project_name}")
        self.logger_library_location_cpp = list(
            self.top_path.glob("**/{}++.so".format(library_name))
        )[0]
        self.logger_library_location = list(
            self.top_path.glob("**/{}.so".format(library_name))
        )[0]
        self.demo_cpp = list(self.top_path.glob("**/demo"))[0]
        self.demo_c = list(self.top_path.glob("**/demo_c"))[0]

    def get_project_top_dir(self):
        """
        Get the git repository's top-level directory.

        Returns:
            str: The git repository's root directory path

        Raises:
            NotADirectoryError: If git command fails to retrieve repository path
        """
        p = Popen(["git", "rev-parse", "--show-toplevel"], stdout=PIPE)
        output = p.communicate()

        for entry in output:
            try:
                return entry.decode("utf-8")
            except TypeError:
                pass
        raise NotADirectoryError


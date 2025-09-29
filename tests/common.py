# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

import pathlib
from subprocess import Popen, PIPE
from inspect import currentframe, getframeinfo, getouterframes


class PathToObjects:
    def __init__(self):
        project_name = 'micro-logger'
        library_name = 'libmicro_logger'
        self.top_path = None
        obj = pathlib.Path.cwd()
        while obj.name:
            if obj.name == project_name:
                self.top_path = obj
                break
            obj = obj.parent
        self.logger_library_location_cpp = list(self.top_path.glob('**/{}++.so'.format(library_name)))[0]
        self.logger_library_location = list(self.top_path.glob('**/{}.so'.format(library_name)))[0]
        self.test_main = list(self.top_path.glob('**/demo'))[0]

    def get_project_top_dir(self):
        p = Popen(['git', 'rev-parse', '--show-toplevel'], stdout=PIPE)
        output = p.communicate()

        for entry in output:
            try:
                return entry.decode('utf-8')
            except NoneType:
                pass
        raise NotADirectoryError


def get_linenumber():
    return currentframe().f_back.f_lineno


def get_function_name():
    return getouterframes(currentframe())[1].function

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

from inspect import currentframe, getouterframes


def get_linenumber():
    return currentframe().f_back.f_lineno


def get_function_name():
    return getouterframes(currentframe())[1].function
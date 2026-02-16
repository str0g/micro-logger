# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

#! /usr/bin/python3
from common import (
    PathToObjects,
    BenchmarkValue,
    DataHandler,
    regex_line_pattern,
    regex_benchmark_pattern,
    get_envrion_variables,
    custom_popen,
    custom_popen_wrapper,
    ThreadedTCPServer,
    NetworkServerRequestHandler,
)
import re
import unittest
import threading

paths = PathToObjects()

g_address = "127.0.0.1"
g_port = 6024


class MsgHelloWorld(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--msg_hello_world"],
            "network": ["-n", "--msg_hello_world"],
        }
        self.expected = DataHandler()
        self.expected.level = "DEBUG"
        self.expected.file = "demo.c"
        self.expected.line = "197"
        self.expected.function = "main"
        self.expected.message = "hello world"


class MsgNull(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--msg_null"],
            "network": ["-n", "--msg_null"],
        }
        self.expected = DataHandler()
        self.expected.level = "ERROR"
        self.expected.file = "demo.c"
        self.expected.line = "200"
        self.expected.function = "main"
        self.expected.message = "(null)"


class MsgTraceEnter(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--msg_trace"],
            "network": ["-n", "--msg_trace"],
        }
        self.expected = DataHandler()
        self.expected.level = "TRACE"
        self.expected.file = "demo.c"
        self.expected.line = "203"
        self.expected.function = "main"
        self.expected.message = "--ENTER--"


class MsgTraceExit(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--msg_trace"],
            "network": ["-n", "--msg_trace"],
        }
        self.expected = DataHandler()
        self.expected.level = "TRACE"
        self.expected.file = "demo.c"
        self.expected.line = "204"
        self.expected.function = "main"
        self.expected.message = "--EXIT--"


class MsgCritical(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--msg_critical"],
            "network": ["-n", "--msg_critical"],
        }
        self.expected = DataHandler()
        self.expected.level = "CRITI"
        self.expected.file = "demo.c"
        self.expected.line = "207"
        self.expected.function = "main"
        self.expected.message = "run out of chocolate for 1 time!"


class MsgThreadsHelloWorld(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--threads"],
            "network": ["-n", "--threads"],
        }
        self.expected = DataHandler()
        self.expected.level = "INFO "
        self.expected.file = "demo.c"
        self.expected.line = "040"
        self.expected.function = "worker_info"
        self.expected.message = "hello "


class MsgThreadsWorldHello(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-o", "--threads"],
            "network": ["-n", "--threads"],
        }
        self.expected = DataHandler()
        self.expected.level = "WARN "
        self.expected.file = "demo.c"
        self.expected.line = "045"
        self.expected.function = "worker_warn"
        self.expected.message = "world "


class CDemoStdOutTesting(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()

    @classmethod
    def setUpClass(cls) -> None:
        cls.maxDiff = None
        cls.binary = paths.demo_c
        cls.option_string = "file"
        cls.env = get_envrion_variables(cls.binary)
        cls.msg_regex = regex_line_pattern

        return super().setUpClass()

    def test_benchmark(self):
        cmd = [self.binary, "--benchmark"]
        regex = re.compile(regex_benchmark_pattern, re.VERBOSE)

        out = []
        with custom_popen(cmd, self.env) as process:
            data = process.stdout.read().decode("utf-8").split("\n")[:-1]
            for line in data:
                match = regex.match(line)
                if not match:
                    raise ValueError("patter not matched")
                out.append(BenchmarkValue(match))

        exp = [
            BenchmarkValue({"time": 402, "size": 45350000, "bandwidth": 108.31}),
            BenchmarkValue({"time": 305, "size": 90700000, "bandwidth": 273.89}),
        ]
        for index, obj in enumerate(exp):
            self.assertEqual(out[index], obj)

    def test_msg_hello_world(self):
        exp = MsgHelloWorld()
        cmd = [self.binary, *exp.options[self.option_string]]
        out = custom_popen_wrapper(cmd, self.env, self.msg_regex, exp)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_msg_null(self):
        exp = MsgNull()
        cmd = [self.binary, *exp.options[self.option_string]]
        out = custom_popen_wrapper(cmd, self.env, self.msg_regex, exp)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_msg_trace(self):
        exp_enter = MsgTraceEnter()
        exp_exit = MsgTraceExit()
        cmd = [self.binary, *exp_enter.options[self.option_string]]
        out = custom_popen_wrapper(cmd, self.env, self.msg_regex, exp_enter)
        self.assertEqual(out[0], exp_enter.expected)
        self.assertEqual(out[1], exp_exit.expected)

    def test_msg_critical(self):
        exp = MsgCritical()
        cmd = [self.binary, *exp.options[self.option_string]]
        out = custom_popen_wrapper(cmd, self.env, self.msg_regex, exp)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_threads(self):
        exp_hello = MsgThreadsHelloWorld()
        exp_world = MsgThreadsWorldHello()
        cmd = [self.binary, *exp_hello.options[self.option_string]]
        outs = custom_popen_wrapper(cmd, self.env, self.msg_regex, exp_hello)
        exp_world.pid = exp_hello.pid
        self.assertEqual(len(outs), 2)
        for out in outs:
            expected = None
            if out.level == "INFO ":
                exp_hello.expected.message += f"{out.tid:016s}"
                expected = exp_hello.expected
            else:
                exp_world.expected.message += f"{out.tid:016s}"
                expected = exp_world.expected
            self.assertEqual(out, expected)


class CDemoNetworkTesting(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()

    @classmethod
    def setUpClass(cls) -> None:
        cls.maxDiff = None
        cls.binary = paths.demo_c
        cls.option_string = "network"
        cls.env = get_envrion_variables(cls.binary)
        cls.msg_regex = regex_line_pattern
        cls.network_server = ThreadedTCPServer(
            (g_address, g_port), NetworkServerRequestHandler
        )
        cls.server_thread = threading.Thread(target=cls.network_server.serve_forever)
        cls.server_thread.start()

        return super().setUpClass()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.network_server.shutdown()
        cls.network_server.server_close()
        cls.server_thread.join()
        return super().tearDownClass()

    def test_msg_hello_world(self):
        exp = MsgHelloWorld()
        cmd = [self.binary, *exp.options[self.option_string]]
        process = custom_popen(cmd, self.env)
        out = NetworkServerRequestHandler.get_output(self.msg_regex)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_msg_null(self):
        exp = MsgNull()
        cmd = [self.binary, *exp.options[self.option_string]]
        process = custom_popen(cmd, self.env)
        out = NetworkServerRequestHandler.get_output(self.msg_regex)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_msg_trace(self):
        exp_enter = MsgTraceEnter()
        exp_exit = MsgTraceExit()
        cmd = [self.binary, *exp_enter.options[self.option_string]]
        process = custom_popen(cmd, self.env)
        out = NetworkServerRequestHandler.get_output(self.msg_regex)
        print("->>")
        print(out)
        print("-<<")
        self.assertEqual(len(out), 2)
        self.assertEqual(out[0], exp_enter.expected)
        self.assertEqual(out[1], exp_exit.expected)

    def test_msg_critical(self):
        exp = MsgCritical()
        cmd = [self.binary, *exp.options[self.option_string]]
        process = custom_popen(cmd, self.env)
        out = NetworkServerRequestHandler.get_output(self.msg_regex)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_threads(self):
        exp_hello = MsgThreadsHelloWorld()
        exp_world = MsgThreadsWorldHello()
        cmd = [self.binary, *exp_hello.options[self.option_string]]
        with custom_popen(cmd, self.env) as process:
            exp_hello.expected.pid = f"{process.pid:08d}"
            exp_world.expected.pid = f"{process.pid:08d}"
            outs = NetworkServerRequestHandler.get_output(self.msg_regex)
            self.assertEqual(len(outs), 2)
            for out in outs:
                expected = None
                if out.level == "INFO ":
                    exp_hello.expected.message += f"{out.tid:016s}"
                    expected = exp_hello.expected
                elif out.level == "WARN ":
                    exp_world.expected.message += f"{out.tid:016s}"
                    expected = exp_world.expected
                else:
                    print(out)
                    self.assertFalse(True)

                self.assertEqual(out, expected)


if __name__ == "__main__":
    unittest.main()

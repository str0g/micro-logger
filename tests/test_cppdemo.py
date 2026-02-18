# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

#! /usr/bin/python3
from common import (
    DataHandler,
    NetworkServerRequestHandler,
    PathToObjects,
    ThreadedTCPServer,
    custom_popen,
    custom_popen_wrapper,
    create_temp_file,
    get_envrion_variables,
    read_and_parse_file,
    regex_line_pattern,
)
import unittest
import threading


paths = PathToObjects()

g_address = "127.0.0.1"
g_port = 6024


class MsgHelloWorld(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "file": ["-f", "--msg_hello_world"],
            "network": ["-n", "--msg_hello_world"],
            "stdout": ["-o", "--msg_hello_world"],
        }
        self.expected = DataHandler()
        self.expected.level = "DEBUG"
        self.expected.file = "demo.cpp"
        self.expected.line = "056"
        self.expected.function = "msg_hello_world"
        self.expected.message = "hello world"


class MsgNull(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_null"],
            "network": ["-n", "--msg_null"],
            "file": ["-f", "--msg_null"],
        }
        self.expected = DataHandler()
        self.expected.level = "ERROR"
        self.expected.file = "demo.cpp"
        self.expected.line = "058"
        self.expected.function = "msg_null"
        self.expected.message = "(null)"


class MsgTraceEnter(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_trace"],
            "network": ["-n", "--msg_trace"],
            "file": ["-f", "--msg_trace"],
        }
        self.expected = DataHandler()
        self.expected.level = "TRACE"
        self.expected.file = "demo.cpp"
        self.expected.line = "061"
        self.expected.function = "msg_trace"
        self.expected.message = "--ENTER--"


class MsgTraceExit(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_trace"],
            "network": ["-n", "--msg_trace"],
            "file": ["-f", "--msg_trace"],
        }
        self.expected = DataHandler()
        self.expected.level = "TRACE"
        self.expected.file = "demo.cpp"
        self.expected.line = "062"
        self.expected.function = "msg_trace"
        self.expected.message = "--EXIT--"


class MsgCritical(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_critical"],
            "network": ["-n", "--msg_critical"],
            "file": ["-f", "--msg_critical"],
        }
        self.expected = DataHandler()
        self.expected.level = "CRITI"
        self.expected.file = "demo.cpp"
        self.expected.line = "066"
        self.expected.function = "msg_critical"
        self.expected.message = "run out of chocolate for 1 time!"


class MsgThreadsHelloWorld(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_threads"],
            "network": ["-n", "--msg_threads"],
            "file": ["-f", "--msg_threads"],
        }
        self.expected = DataHandler()
        self.expected.level = "INFO "
        self.expected.file = "demo.cpp"
        self.expected.line = "049"
        self.expected.function = "operator()"
        self.expected.message = "hello "


class MsgThreadsWorldHello(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_threads"],
            "network": ["-n", "--msg_threads"],
            "file": ["-f", "--msg_threads"],
        }
        self.expected = DataHandler()
        self.expected.level = "WARN "
        self.expected.file = "demo.cpp"
        self.expected.line = "044"
        self.expected.function = "worker_warn"
        self.expected.message = "world "


class MsgSingletonEnter(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_singleton"],
            "network": ["-n", "--msg_singleton"],
            "file": ["-f", "--msg_singleton"],
        }
        self.expected = DataHandler()
        self.expected.level = "TRACE"
        self.expected.file = "demo.cpp"
        self.expected.line = "076"
        self.expected.function = "~SampleSingletonClass"
        self.expected.message = "--ENTER--"


class MsgSingletonExit(DataHandler):
    def __init__(self, match=None) -> None:
        super().__init__(match)
        self.options = {
            "stdout": ["-o", "--msg_singleton"],
            "network": ["-n", "--msg_singleton"],
            "file": ["-f", "--msg_singleton"],
        }
        self.expected = DataHandler()
        self.expected.level = "TRACE"
        self.expected.file = "demo.cpp"
        self.expected.line = "077"
        self.expected.function = "~SampleSingletonClass"
        self.expected.message = "--EXIT--"


class IncorrectInvocation(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.maxDiff = None
        cls.binary = paths.demo_cpp
        cls.env = get_envrion_variables(cls.binary)

        return super().setUpClass()

    def test_crash(self):
        cmd = [self.binary, "--crash"]
        with custom_popen(cmd, self.env) as process:
            self.assertTrue(
                "AddressSanitizer:DEADLYSIGNAL" in process.stderr.read().decode("utf-8")
            )

    def test_redirect_to_help(self):
        for option in [
            "--msg_hello_world",
            "--msg_null",
            "--msg_trace",
            "--msg_critical",
            "--msg_threads",
            "--msg_singleton",
        ]:
            cmd = [self.binary, option]
            with custom_popen(cmd, self.env) as process:
                self.assertTrue("Usage:" in process.stderr.read().decode("utf-8"))


class CppDemoStdOutTesting(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.maxDiff = None
        cls.binary = paths.demo_cpp
        cls.option_string = "stdout"
        cls.env = get_envrion_variables(cls.binary)
        cls.msg_regex = regex_line_pattern

        return super().setUpClass()

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

    def test_msg_singleton(self):
        exp_enter = MsgSingletonEnter()
        exp_exit = MsgSingletonExit()
        cmd = [self.binary, *exp_enter.options[self.option_string]]
        out = custom_popen_wrapper(cmd, self.env, self.msg_regex, exp_enter)
        self.assertEqual(len(out), 2)
        self.assertEqual(out[0], exp_enter.expected)
        self.assertEqual(out[1], exp_exit.expected)


class CppDemoNetworkTesting(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.maxDiff = None
        cls.binary = paths.demo_cpp
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

    def test_msg_singleton(self):
        exp_enter = MsgSingletonEnter()
        exp_exit = MsgSingletonExit()
        cmd = [self.binary, *exp_enter.options[self.option_string]]
        process = custom_popen(cmd, self.env)
        out = NetworkServerRequestHandler.get_output(self.msg_regex)
        self.assertEqual(len(out), 2)
        self.assertEqual(out[0], exp_enter.expected)
        self.assertEqual(out[1], exp_exit.expected)


class CppDemoFileTesting(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.maxDiff = None
        cls.binary = paths.demo_cpp
        cls.option_string = "file"
        cls.env = get_envrion_variables(cls.binary)
        cls.msg_regex = regex_line_pattern
        cls.default_file = create_temp_file()

        return super().setUpClass()

    def test_msg_hello_world(self):
        exp = MsgHelloWorld()
        options = exp.options[self.option_string]
        cmd = [self.binary, f"{options[0]}{self.default_file}", options[1]]
        with custom_popen(cmd, self.env) as process:
            exp.expected.pid = f"{process.pid:08d}"
            out = read_and_parse_file(self.default_file, self.msg_regex)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_msg_null(self):
        exp = MsgNull()
        options = exp.options[self.option_string]
        cmd = [self.binary, f"{options[0]}{self.default_file}", options[1]]
        with custom_popen(cmd, self.env) as process:
            exp.expected.pid = f"{process.pid:08d}"
            out = read_and_parse_file(self.default_file, self.msg_regex)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_msg_trace(self):
        exp_enter = MsgTraceEnter()
        exp_exit = MsgTraceExit()
        options = exp_enter.options[self.option_string]
        cmd = [self.binary, f"{options[0]}{self.default_file}", options[1]]
        with custom_popen(cmd, self.env) as process:
            exp_enter.expected.pid = f"{process.pid:08d}"
            exp_exit.expected.pid = f"{process.pid:08d}"
            out = read_and_parse_file(self.default_file, self.msg_regex)
        self.assertEqual(len(out), 2)
        self.assertEqual(out[0], exp_enter.expected)
        self.assertEqual(out[1], exp_exit.expected)

    def test_msg_critical(self):
        exp = MsgCritical()
        options = exp.options[self.option_string]
        cmd = [self.binary, f"{options[0]}{self.default_file}", options[1]]
        with custom_popen(cmd, self.env) as process:
            exp.expected.pid = f"{process.pid:08d}"
            out = read_and_parse_file(self.default_file, self.msg_regex)
        self.assertEqual(len(out), 1)
        self.assertEqual(out[0], exp.expected)

    def test_threads(self):
        exp_hello = MsgThreadsHelloWorld()
        exp_world = MsgThreadsWorldHello()
        options = exp_hello.options[self.option_string]
        cmd = [self.binary, f"{options[0]}{self.default_file}", options[1]]
        with custom_popen(cmd, self.env) as process:
            exp_hello.expected.pid = f"{process.pid:08d}"
            exp_world.expected.pid = f"{process.pid:08d}"
            out = read_and_parse_file(self.default_file, self.msg_regex)
            self.assertEqual(len(out), 2)
            for out_msg in out:
                expected = None
                if out_msg.level == "INFO ":
                    exp_hello.expected.message += f"{out_msg.tid:016s}"
                    expected = exp_hello.expected
                else:
                    exp_world.expected.message += f"{out_msg.tid:016s}"
                    expected = exp_world.expected
                self.assertEqual(out_msg, expected)

    def test_msg_singleton(self):
        exp_enter = MsgSingletonEnter()
        exp_exit = MsgSingletonExit()
        options = exp_enter.options[self.option_string]
        cmd = [self.binary, f"{options[0]}{self.default_file}", options[1]]
        with custom_popen(cmd, self.env) as process:
            exp_enter.expected.pid = f"{process.pid:08d}"
            exp_exit.expected.pid = f"{process.pid:08d}"
            out = read_and_parse_file(self.default_file, self.msg_regex)
        self.assertEqual(len(out), 2)
        self.assertEqual(out[0], exp_enter.expected)
        self.assertEqual(out[1], exp_exit.expected)


if __name__ == "__main__":
    unittest.main()

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

#! /usr/bin/python3
import re
import unittest
import socketserver
import ctypes
from subprocess import Popen, PIPE
import pathlib
from threading import Thread
import time
import re

#c_lib = ctypes.CDLL(paths.logger_library_location)

class PathToObjects:
    def __init__(self):
        project_name = "micro-logger"
        self.top_path = None
        obj = pathlib.Path.cwd()
        while obj.name:
            if obj.name == project_name:
                self.top_path = obj
                break
            obj = obj.parent
        self.logger_library_location_cpp = list(self.top_path.glob("**/*.so"))[0]
        self.logger_library_location = list(self.top_path.glob("**/*.so"))[0]
        self.test_main = list(self.top_path.glob("**/main"))[0]

    def get_project_top_dir(self):
        p = Popen(['git', 'rev-parse', '--show-toplevel'], stdout=PIPE)
        output = p.communicate()

        for entry in output:
            try:
                return entry.decode("utf-8")
            except NoneType:
                pass
        raise NotADirectoryError


paths = PathToObjects()


class MsgTraceHandler(socketserver.BaseRequestHandler):
    def handle(self):
        data = self.request.recv(1024)
        tmp_data = data.decode('utf-8')
        index = tmp_data.find('\n')
        line_enter = tmp_data[:index+1]

        #print(line_enter)
        regex = r"(\[[0-9]{2}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\])(\[TRACE\])(\[pid:[A-Za-z0-9]{8}])(\[tid:[A-Za-z0-9]{16}\])(\[main.cpp:[0-9]{3}::msg_trace])(\[--ENTER--])\n"
        matches = re.search(regex, line_enter)
        if not matches:
            raise ValueError("enter not found")

        if len(tmp_data) > 100:
            line_exit = tmp_data[index:]
        else:
            data = self.request.recv(1024)
            line_exit = data.decode('utf-8')
        #print(line_exit)
        regex = r"(\[[0-9]{2}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\])(\[TRACE\])(\[pid:[A-Za-z0-9]{8}])(\[tid:[A-Za-z0-9]{16}\])(\[main.cpp:[0-9]{3}::msg_trace])(\[--EXIT--])\n"
        matches = re.search(regex, line_exit)
        if not matches:
            raise ValueError("exit not found")


class ThreadsHandler(socketserver.BaseRequestHandler):
    def get_tid_value(self, buf, pattern):
        index = buf.find(pattern)
        if (index == -1):
            raise LookupError
        index += len(pattern)
        return buf[index:index+16]

    def handle(self):
        data = self.request.recv(1024)
        tmp_data = data.decode('utf-8')
        index = tmp_data.find('\n')
        line_warn = tmp_data[:index+1]

        #print(line_warn)
        regex = "(\[[0-9]{2}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\])(\[(WARN|INFO) \])(\[pid:[A-Za-z0-9]{8}])(\[tid:[A-Za-z0-9]{16}\])(\[main.cpp:[0-9]{3}::operator\(\)])(\[(world|hello) [A-Za-z0-9]{16}\])\n"
        matches = re.search(regex, line_warn)
        if not matches:
            raise ValueError("world not found")

        tid = self.get_tid_value(line_warn, 'tid:')
        try:
            msg_tid = self.get_tid_value(line_warn, 'world ')
        except LookupError:
            msg_tid = self.get_tid_value(line_warn, 'hello ')

        if tid != msg_tid:
            raise ValueError("tid did not match in message")

        if len(tmp_data) > 100:
            line_info = tmp_data[index:]
        else:
            # sometime may not pass @todo further investigation needed
            time.sleep(0.3)
            line_info = self.request.recv(1024).decode('utf-8')
        #print("->", line_info)
        matches = re.search(regex, line_info)
        if not matches:
            raise ValueError("hello not found")

        tid = self.get_tid_value(line_info, 'tid:')
        try:
            msg_tid = self.get_tid_value(line_info, 'hello ')
        except LookupError:
            msg_tid = self.get_tid_value(line_info, 'world ')
        if tid != msg_tid:
            raise ValueError("tid did not match in message")

        if line_warn == line_info:
            raise ValueError("parsing the same data fix test or logger")


class MsgHelloWroldHandler(socketserver.BaseRequestHandler):
    def handle(self):
        data = self.request.recv(1024)
        #print(data)
        regex = r"(\[[0-9]{2}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\])(\[DEBUG\])(\[pid:[A-Za-z0-9]{8}])(\[tid:[A-Za-z0-9]{16}\])(\[main.cpp:[0-9]{3}::msg_hello_world])(\[hello world])\n"
        matches = re.search(regex, data.decode('utf-8'))
        if not matches:
            raise ValueError("hello world not found")


class MsgNullHandler(socketserver.BaseRequestHandler):
    def handle(self):
        data = self.request.recv(1024)
        #print(data)
        regex = r"(\[[0-9]{2}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\])(\[ERROR\])(\[pid:[A-Za-z0-9]{8}])(\[tid:[A-Za-z0-9]{16}\])(\[main.cpp:[0-9]{3}::msg_null])(\[\(null\)])\n"
        matches = re.search(regex, data.decode('utf-8'))
        if not matches:
            raise ValueError("null not found")


class MsgCriticalHandler(socketserver.BaseRequestHandler):
    def handle(self):
        data = self.request.recv(1024)
        #print(data)
        regex = "(\[[0-9]{2}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\])(\[CRITI\])(\[pid:[A-Za-z0-9]{8}])(\[tid:[A-Za-z0-9]{16}\])(\[main.cpp:[0-9]{3}::msg_critical])(\[run out of chocolate for 1 time!)\]\n"
        matches = re.search(regex, data.decode('utf-8'))
        if not matches:
            raise ValueError("critical not found")


def test_main_exec(writer='net', test='all', sleep=0.1):
    time.sleep(sleep)
    p = Popen([paths.test_main, writer, test], stdout=PIPE)
    output = p.communicate()
    return output


class CppAppTesting(unittest.TestCase):
    def base_fun(self, writer, test, handler):
        t1 = Thread(target=test_main_exec, args=(writer, test))
        t1.start()
        with socketserver.TCPServer(('127.0.0.1', 6024), handler) as server:
            server.allow_reuse_address = True
            server.timeout = 0.5
            server.handle_request()
        t1.join()

    def test_msg_trace(self):
        self.base_fun('net', 'msg_trace', MsgTraceHandler)

    def test_threads(self):
        self.base_fun('net', 'threads', ThreadsHandler)

    def test_msg_hello_world(self):
        self.base_fun('net', 'msg_hello_world', MsgHelloWroldHandler)

    def test_msg_null(self):
        self.base_fun('net', 'msg_null', MsgNullHandler)

    def test_msg_critical(self):
        self.base_fun('net', 'msg_critical', MsgCriticalHandler)


if __name__ == '__main__':
    unittest.main()

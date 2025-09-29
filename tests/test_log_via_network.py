# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

#! /usr/bin/python3
import re
import unittest
import socketserver
from subprocess import Popen, PIPE
from threading import Thread
import time
import re
#
from common import PathToObjects

paths = PathToObjects()

regex_pattern = r"""
\[(?P<data>\d{2}/\d{2}/\d{2})[ ](?P<time>\d{2}:\d{2}:\d{2}\.\d{3})\]
\[(?P<level>(TRACE|DEBUG|INFO[ ]|WARN[ ]|ERROR|CRITI))\]
\[pid:(?P<pid>\d{8})\]\[tid:(?P<tid>\d{16})\]
\[(?P<file>[a-zA-Z0-9_-]+\.[cp]{1,3}):(?P<line>\d{1,4})::(?P<function>.*?)\]\[(?P<message>.*?)\]
"""

g_out = []

def patern_recive_data_and_store_level_message(self):
   data = self.request.recv(1024)
   regex = re.compile(regex_pattern, re.VERBOSE)
   match = regex.match(data.decode('utf-8'))
   if not match:
       raise ValueError('patter not matched')
   g_out.append(match['level'])
   g_out.append(match['message'])


class MsgTraceHandler(socketserver.BaseRequestHandler):
    allow_reuse_address = True

    def handle(self):
        data = self.request.recv(1024)
        tmp_data = data.decode('utf-8')
        index = tmp_data.find('\n')
        line = tmp_data[:index+1]

        regex = re.compile(regex_pattern, re.VERBOSE)
        match = regex.match(line)
        if not match:
            raise ValueError('patter not matched')
        g_out.append(match['level'])
        g_out.append(match['message'])

        if len(tmp_data) > 200:
            line = tmp_data[index:]
        else:
            data = self.request.recv(1024)
            line = data.decode('utf-8')
        match = regex.match(line)
        if not match:
            raise ValueError('patter not matched')
        g_out.append(match['level'])
        g_out.append(match['message'])


class ThreadsHandler(socketserver.BaseRequestHandler):
    allow_reuse_address = True

    def handle(self):
        data = self.request.recv(1024)
        tmp_data = data.decode('utf-8')
        index = tmp_data.find('\n')
        line = tmp_data[:index+1]

        regex = re.compile(regex_pattern, re.VERBOSE)
        match = regex.match(line)
        if not match:
            raise ValueError('patter not matched')
        g_out.append(match['level'])
        msg = match["message"]
        g_out.append(msg[:msg.rfind(' ')])
        pid = match['pid']
        tid = match['tid']

        if len(tmp_data) > 200:
            line_info = tmp_data[index:]
        else:
            # sometime may not pass @todo further investigation needed
            line_info = self.request.recv(1024).decode('utf-8')
        match = regex.match(line_info)
        if not match:
            raise ValueError('patter not matched')
        g_out.append(match['level'])
        msg = match["message"]
        g_out.append(msg[:msg.rfind(' ')])
        if pid != match['pid']:
            g_out.append("pids are different")
        if(tid == match['tid']):
            g_out.append("tids are equal")


class MsgHelloWroldHandler(socketserver.BaseRequestHandler):
    allow_reuse_address = True

    def handle(self):
        patern_recive_data_and_store_level_message(self)


class MsgNullHandler(socketserver.BaseRequestHandler):
    allow_reuse_address = True

    def handle(self):
        patern_recive_data_and_store_level_message(self)


class MsgCriticalHandler(socketserver.BaseRequestHandler):
    allow_reuse_address = True

    def handle(self):
        patern_recive_data_and_store_level_message(self)


def test_main_exec(writer='net', test='all', sleep=0.1):
    time.sleep(sleep)
    p = Popen([paths.test_main, writer, test], stdout=PIPE)
    output = p.communicate()
    return output


class CppDemoTesting(unittest.TestCase):
    def setUp(self) -> None:
        g_out.clear()
        return super().setUp()

    def base_fun(self, writer, test, handler):
        t1 = Thread(target=test_main_exec, args=(writer, test))
        t1.start()
        with socketserver.TCPServer(('127.0.0.1', 6024), handler) as server:
            server.handle_request()
        t1.join()

    def test_msg_trace(self):
        self.base_fun('net', 'msg_trace', MsgTraceHandler)
        exp = ['TRACE', '--ENTER--', 'TRACE', '--EXIT--']
        self.assertEqual(g_out, exp)

    def test_threads(self):
        self.base_fun('net', 'threads', ThreadsHandler)
        exp = ['INFO ', 'hello', 'WARN ', 'world']
        self.assertEqual(set(g_out), set(exp))

    def test_msg_hello_world(self):
        self.base_fun('net', 'msg_hello_world', MsgHelloWroldHandler)
        exp = ['DEBUG', 'hello world']
        self.assertEqual(g_out, exp)

    def test_msg_null(self):
        self.base_fun('net', 'msg_null', MsgNullHandler)
        exp = ['ERROR', '(null)']
        self.assertEqual(g_out, exp)

    def test_msg_critical(self):
        self.base_fun('net', 'msg_critical', MsgCriticalHandler)
        exp = ['CRITI' , "run out of chocolate for 1 time!"]
        self.assertEqual(g_out, exp)


if __name__ == '__main__':
    unittest.main()

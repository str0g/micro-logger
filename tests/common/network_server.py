# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

import threading
import socketserver

from .data_handler import DataHandler
from .custom_popen import match_line_data

g_out = []
g_cv = threading.Condition()


class NetworkServerRequestHandler(socketserver.BaseRequestHandler):
    """TCP socket handler for receiving network log messages"""

    def handle(self):
        """Receive data via socket and parse using regex"""
        global g_cv
        global g_out
        try:
            while True:
                data = self.request.recv(65536)  # Receive up to 64KB
                if not data:
                    break
                print("-> ", data)
                g_out.append(data.decode("utf-8"))
        except Exception as e:
            print(f"Error handling network data: {e}")
        finally:
            if len(g_out):
                with g_cv:
                    g_cv.notify_all()
            self.request.close()

    @staticmethod
    def fun_to_get_g_out_len():
        return len(g_out)

    @staticmethod
    def get_output(msg_regex):
        global g_cv
        global g_out
        with g_cv:
            g_cv.wait_for(NetworkServerRequestHandler.fun_to_get_g_out_len, timeout=1)
            out = []
            for o in g_out:
                for line in o.split("\n")[:-1]:
                    out.append(DataHandler(match_line_data(line, msg_regex)))
            g_out.clear()
            return out


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = True
    pass


#! /usr/bin/env python3
import re
import argparse
import tornado.gen
import tornado.ioloop
import tornado.tcpclient
from pathlib import Path


class DemoClient:
    def __init__(self, host, port, delay):
        self.host = host
        self.port = port
        self.delay = delay

    async def send_file(self, filename):
        connection_pool = dict()  # client_id -> connection
        address_pool = dict()
        filepath = Path(filename)
        with filepath.open() as f:
            for line in f:
                match = re.match(r"(\d+):(.*)", line)
                if match is None:
                    continue
                await tornado.gen.sleep(self.delay)
                client_id = match.group(1)
                command = match.group(2) + "\n"

                if client_id not in connection_pool.keys():
                    client = tornado.tcpclient.TCPClient()
                    try:
                        connection = await client.connect(self.host, self.port)
                        connection_pool[client_id] = connection
                        host, port = connection.socket.getsockname()
                        address_pool[f"{host}:{port}"] = f"[address{client_id}]"
                    except:
                        print("connection failed")
                        return

                connection = connection_pool[client_id]
                data = await connection.read_until(b"% ")
                output = data.decode()
                prefixed = '\n'.join(
                    f"{client_id}| {line}" for line in output.split("\n"))

                for address, alias in address_pool.items():
                    prefixed = prefixed.replace(address, alias)

                print(prefixed, end="")
                await connection.write(command.encode())
                print(command, end="")


async def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", type=str, required=True, help="server host")
    parser.add_argument("--port", type=int, required=True, help="server port")
    parser.add_argument("--testcase", type=str, required=True, help="testcase path")
    parser.add_argument("--delay", type=float, default=0.2, help="Sleep time per command")
    args = parser.parse_args()

    client = DemoClient(args.host, args.port, args.delay)
    await client.send_file(args.testcase)


if __name__ == "__main__":
    tornado.ioloop.IOLoop.current().run_sync(main)

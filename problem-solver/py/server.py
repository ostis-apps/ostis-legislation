import argparse
import time
import sys
import threading
from threading import Thread
import sc_kpm
from sc_client.client import create_elements, delete_elements
from sc_client.models import ScConstruction
from sc_client.constants import sc_types
from sc_kpm import ScServer
from sc_kpm.utils import create_node
import signal
from modules.AgentProcessingModule import AgentProcessingModule
from shutdown_manager import shutdown_manager


import logging
logging.basicConfig(level=logging.DEBUG)


stop_event = threading.Event()
SC_SERVER_PROTOCOL = "protocol"
SC_SERVER_HOST = "host"
SC_SERVER_PORT = "port"

SC_SERVER_PROTOCOL_DEFAULT = "ws"
SC_SERVER_HOST_DEFAULT = "localhost"
SC_SERVER_PORT_DEFAULT = "8090"


def init_agent():
    time.sleep(2.5)

    construction = ScConstruction()  # First you need initialize

    action_initiated_addr = sc_kpm.ScKeynodes['action_initiated']
    telegram_addr = sc_kpm.ScKeynodes['action_start_agent']
    action_addr = sc_kpm.ScKeynodes['action']

    class_node = create_node(sc_types.NODE_CONST)
    construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, telegram_addr, class_node)

    construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, telegram_addr, action_initiated_addr)
    construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated_addr, class_node)
    construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_addr, class_node)
    addrs = create_elements(construction)


def signal_handler(sig, frame):
    shutdown_manager.stop()


def main(args: dict):
    signal.signal(signal.SIGINT, signal_handler)
    server = ScServer(f"{args['protocol']}://{args['host']}:{args['port']}")

    try:
        with server.connect():
            modules = [AgentProcessingModule()]
            server.add_modules(*modules)
            init_thread = Thread(target=init_agent)
            init_thread.start()
            with server.register_modules():
                while not shutdown_manager.is_stopped():
                    time.sleep(0.5)
            init_thread.join()

    except Exception as e:
        logging.error(f"Error in main execution: {e}")
    finally:
        logging.info("Shutting down server...")
        server.serve()



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--protocol', type=str, dest=SC_SERVER_PROTOCOL, default=SC_SERVER_PROTOCOL_DEFAULT, help="Sc-server protocol")
    parser.add_argument(
        '--host', type=str, dest=SC_SERVER_HOST, default=SC_SERVER_HOST_DEFAULT, help="Sc-server host")
    parser.add_argument(
        '--port', type=int, dest=SC_SERVER_PORT, default=SC_SERVER_PORT_DEFAULT, help="Sc-server port")
    args = parser.parse_args()

    main(vars(args))
import argparse
import time
import threading
from threading import Thread
import sc_kpm
from sc_client.client import generate_elements
from sc_client.models import ScConstruction
from sc_kpm import ScServer
from sc_kpm.utils import generate_node
from agents.modules.agent_processing_module import AgentProcessingModule
from sc_client.constants import sc_type
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
    time.sleep(1)
    construction = ScConstruction()
    action_initiated_addr = sc_kpm.ScKeynodes['action_initiated']
    telegram_addr = sc_kpm.ScKeynodes['action_start_agent']
    action_addr = sc_kpm.ScKeynodes['action']
    class_node = generate_node(sc_type.CONST_NODE)
    construction.generate_connector(sc_type.CONST_PERM_POS_ARC, telegram_addr, class_node)
    construction.generate_connector(sc_type.CONST_PERM_POS_ARC, telegram_addr, action_initiated_addr)
    construction.generate_connector(sc_type.CONST_PERM_POS_ARC, action_initiated_addr, class_node)
    construction.generate_connector(sc_type.CONST_PERM_POS_ARC, action_addr, class_node)
    generate_elements(construction)



def main(args: dict):
    server = ScServer(f"{args['protocol']}://{args['host']}:{args['port']}")
    with server.connect():
        modules = [
            AgentProcessingModule()
        ]
        server.add_modules(*modules)
        init_thread = Thread(target=init_agent)
        init_thread.start()
        with server.register_modules():
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
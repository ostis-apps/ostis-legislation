import argparse
import time
from argparse import Action
from threading import Thread

import sc_kpm
from sc_client.client import create_elements
from sc_client.models import ScConstruction
from sc_client.constants import sc_types
from sc_kpm import ScServer, ScAgentClassic
from sc_kpm.identifiers import ActionStatus
from sc_kpm.utils import create_node, create_edge

from modules.AgentProcessingModule import AgentProcessingModule

SC_SERVER_PROTOCOL = "protocol"
SC_SERVER_HOST = "host"
SC_SERVER_PORT = "port"

SC_SERVER_PROTOCOL_DEFAULT = "ws"
SC_SERVER_HOST_DEFAULT = "localhost"
SC_SERVER_PORT_DEFAULT = "8090"


def init_agent():
    time.sleep(2.5)
    construction = ScConstruction()
    action_initiated_addr = sc_kpm.ScKeynodes[ActionStatus.ACTION_INITIATED]
    agent_call_addr = sc_kpm.ScKeynodes['telegram_start_agent']
    agent_node = create_node(sc_types.NODE_CONST)
    construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, agent_call_addr, agent_node)

    construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated_addr, agent_node)
    #construction.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated_addr, agent_call_addr)
    addrs = create_elements(construction)

def main(args: dict):
    server = ScServer(
        f"{args[SC_SERVER_PROTOCOL]}://{args[SC_SERVER_HOST]}:{args[SC_SERVER_PORT]}")
    with server.connect():
        modules = [
            AgentProcessingModule()
        ]
        server.add_modules(*modules)
        thread = Thread(target = init_agent)
        thread.start()
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
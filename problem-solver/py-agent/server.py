import argparse
import threading
from modules.telegram_data import start_bot
from sc_kpm import ScServer
from modules.MessageProcessingModule import MessageProcessingModule

from pathlib import Path
from modules.ScTestTelegramAgent import TestScAgent
SC_SERVER_PROTOCOL = "protocol"
SC_SERVER_HOST = "host"
SC_SERVER_PORT = "port"

SC_SERVER_PROTOCOL_DEFAULT = "ws"
SC_SERVER_HOST_DEFAULT = "localhost"
SC_SERVER_PORT_DEFAULT = "8090"


def main(args: dict):
    server = ScServer(
        f"{args[SC_SERVER_PROTOCOL]}://{args[SC_SERVER_HOST]}:{args[SC_SERVER_PORT]}")

    with server.connect():
        modules = [
            MessageProcessingModule()
        ]
        server.add_modules(*modules)
        with server.register_modules():
            bot_thread = threading.Thread(target=start_bot)
            bot_thread.start()
            if bot_thread.is_alive():
                print("Бот успешно запущен.")
            else:
                print("Не удалось запустить бота.")
            bot_thread.join()


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
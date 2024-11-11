import logging
import time
from sc_client.models import ScAddr
from sc_kpm import ScAgentClassic, ScResult
from modules.telegram_data import start_bot
import threading
#from shutdown_manager import shutdown_manager


logging.basicConfig(level=logging.INFO)


class TelegramScAgent(ScAgentClassic):
    def __init__(self) -> None:
        super().__init__("action_start_agent")

    def on_event(self, event_element: ScAddr, event_edge: ScAddr, action_element: ScAddr) -> ScResult:
        self.logger.info("TG Agent was called")
        result = self.__run()
        self.logger.info("TG Agent finished %s")
        return result

    def __run(self) -> ScResult:
        self.logger.info("TG Agent began to run")
        bot_thread = threading.Thread(target=start_bot)
        bot_thread.start()

        if bot_thread.is_alive():
            print("Бот успешно запущен.")
        else:
            print("Не удалось запустить бота.")
        #while not shutdown_manager.is_stopped():
            time.sleep(0.5)
        #bot_thread.join()

        self.logger.info("Bot started by TG Agent")
        return ScResult.OK
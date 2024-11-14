import logging
import time

import sc_kpm
from sc_client.client import get_link_content
from sc_client.models import ScAddr
from sc_client.constants import sc_types
from sc_kpm import ScAgentClassic, ScResult, ScKeynodes
from modules.telegram_data import start_bot
import threading

from sc_kpm.sc_sets import ScStructure, ScNumberedSet
from sc_kpm.utils import create_link
import sc_kpm.utils as utils

#from shutdown_manager import shutdown_manager


logging.basicConfig(level=logging.INFO)


class TelegramScAgent(ScAgentClassic):
    def __init__(self) -> None:
        super().__init__("action_start_agent")



    def on_event(self, event_element: ScAddr, event_edge: ScAddr, action_element: ScAddr) -> ScResult:
        self.logger.info("TG Agent was called")
        result = self.__run()
        is_successful = result == ScResult.OK
        self.logger.info("TG Agent finished %s")
        return result

    def __run(self) -> ScResult:
        self.logger.info("TG Agent began to run")
        bot_thread = threading.Thread(target=start_bot)
        bot_thread.start()

        question_instance = utils.action_utils.create_action("action", "action_generate_questions")
        utils.create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, question_instance, ScKeynodes.get("question_model"))

        self.logger.info("Bot finished by TG Agent")
        return ScResult.OK
#todo: возвращать все вопросы из агента
#todo: уточнить как получить из класса всё что хочу
#todo: упаковать и передавать всё полученное в ScTestQuestionClass
#todo: profit
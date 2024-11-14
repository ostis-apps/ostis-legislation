import logging
from tabnanny import check

from sc_client.client import create_elements, generate_elements
import time
import sc_kpm
from typing import Dict, List, Tuple, Union
from sc_client.models import ScAddr, ScConstruction
from sc_client.constants import sc_types
from sc_kpm import ScAgentClassic, ScResult, ScKeynodes
from modules.telegram_data import start_bot
import threading
import sc_kpm.utils as utils
from sc_kpm.identifiers import CommonIdentifiers, ActionStatus
from sc_kpm.utils.action_utils import execute_agent, COMMON_WAIT_TIME, create_action
from sc_kpm.utils import create_node, check_edge, create_role_relation, create_edge

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

    def execute_generate_question_agent(self) -> Tuple[list[ScAddr], bool]:

        construction = ScConstruction()
        action_initiated_addr = sc_kpm.ScKeynodes['action_initiated']
        action_gen_questions = sc_kpm.ScKeynodes['action_generate_questions']
        question_model_addr = sc_kpm.ScKeynodes['question_model']
        action_addr = sc_kpm.ScKeynodes['action']
        class_node = sc_types.NODE_CONST
        rrel_node = sc_types.NODE_CONST_ROLE

        construction.generate_connector(sc_types.EDGE_ACCESS_CONST_POS_PERM,action_gen_questions, class_node)

        #sync with KB

        construction.generate_connector(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated_addr, class_node)
        construction.generate_connector(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_addr, class_node)
        addrs = generate_elements(construction)

        result = check_edge(sc_types.EDGE_ACCESS_VAR_POS_PERM, action_addr, class_node)
        return addrs, result

    def __run(self) -> ScResult:
        self.logger.info("TG Agent began to run")
        bot_thread = threading.Thread(target=start_bot)
        bot_thread.start()

        print(ScKeynodes["question_model"])

        action_initiated = sc_kpm.ScKeynodes["action_initiated"]
        class_node = create_node(sc_types.NODE_CONST)
        question_node = sc_kpm.ScKeynodes["action_generate_questions"]
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, question_node, class_node)

        utils.create_role_relation(class_node, ScKeynodes.get("question_model"), ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated, class_node)



        self.logger.info("Bot finished by TG Agent")
        return ScResult.OK

#todo: возвращать все вопросы из агента
#todo: уточнить как получить из класса всё что хочу
#todo: упаковать и передавать всё полученное в ScTestQuestionClass
#todo: profit
import logging
from re import search
from tabnanny import check

from sc_client.client import create_elements, generate_elements, search_by_template, get_link_content, delete_elements
import time
import sc_kpm
from typing import Dict, List, Tuple, Union
from sc_client.models import ScAddr, ScConstruction, ScTemplateResult, ScTemplate
from sc_client.constants import sc_types
from sc_kpm import ScAgentClassic, ScResult, ScKeynodes
from sc_kpm.sc_sets import ScStructure, ScNumberedSet, ScSet

from modules.telegram_data import start_bot
import threading
import sc_kpm.utils as utils
from sc_kpm.identifiers import CommonIdentifiers, ActionStatus
from sc_kpm.utils.action_utils import execute_agent, COMMON_WAIT_TIME, create_action
from sc_kpm.utils import create_node, check_edge, create_role_relation, create_edge, get_element_by_role_relation, \
    get_element_by_norole_relation

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
        print(ScKeynodes["question_model"])

        action_initiated = sc_kpm.ScKeynodes["action_initiated"]
        class_node = create_node(sc_types.NODE_VAR)
        question_node = sc_kpm.ScKeynodes["action_generate_questions"]
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, question_node, class_node)

        utils.create_role_relation(class_node, ScKeynodes.get("question_model"), ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated, class_node)


        template = ScTemplate()
        template.quintuple(
            class_node,
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.NODE_VAR_STRUCT,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            sc_types.NODE_VAR_NOROLE
        )
        search = search_by_template(template)
        get_struct = search[0].get(2)

        result_temp = ScTemplate()
        result_temp.triple(
            get_struct,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            sc_types.NODE_VAR_TUPLE
        )
        result_search = search_by_template(result_temp)
        result_search = result_search[0].get(2)

        result_set = ScNumberedSet(set_node = result_search)

        nrel_gen_question = sc_kpm.ScKeynodes["nrel_generated_question_text"]
        nrel_corr_answer = sc_kpm.ScKeynodes["nrel_generated_question_correct_answer"]

        question_form = ScTemplate()
        question_form.quintuple(
            result_set[0],
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.LINK_VAR,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            nrel_gen_question
        )
        question_search = search_by_template(question_form)

        answer_form = ScTemplate()
        answer_form.quintuple(
            result_set[0],
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.LINK_VAR,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            nrel_corr_answer
        )
        answer_search = search_by_template(answer_form)


        first = get_link_content(question_search[0].get(2))
        second = get_link_content(answer_search[0].get(2))

        self.logger.info("Bot finished by TG Agent")
        return ScResult.OK

#todo: добавить итератор и пихать в словарь
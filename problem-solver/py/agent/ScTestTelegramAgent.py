import logging
import telebot
from ScTestQuestionClass import ScTestQuestionClass
from sc_client.client import search_by_template, get_link_content
import time
import sc_kpm
from sc_client.models import ScAddr, ScTemplate
from sc_client.constants import sc_types
from sc_kpm import ScAgentClassic, ScResult, ScKeynodes
from sc_kpm.sc_sets import ScNumberedSet
import threading
import sc_kpm.utils as utils
from sc_kpm.utils import create_node, create_edge

logging.basicConfig(level=logging.INFO)
bot = telebot.TeleBot('7779388088:AAEtaxwQcH43XNAuKuHLRFZsWDdtObTH__Q')
question_list = []

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


        class_node = create_node(sc_types.NODE_VAR)
        action_initiated = sc_kpm.ScKeynodes["action_initiated"]
        question_node = sc_kpm.ScKeynodes["action_generate_questions"]
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, question_node, class_node)

        utils.create_role_relation(class_node, ScKeynodes.get("question_model"), ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated, class_node)
        time.sleep(5)

        template = ScTemplate()
        template.quintuple(
            class_node,
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.NODE_VAR_STRUCT,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            sc_types.NODE_VAR_NOROLE
        )
        search_struct = search_by_template(template)
        get_struct = search_struct[0].get(2)

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
        for _ in range(10):
            question_form = ScTemplate()
            question_form.quintuple(
                result_set[_],
                sc_types.EDGE_D_COMMON_VAR,
                sc_types.LINK_VAR,
                sc_types.EDGE_ACCESS_VAR_POS_PERM,
                nrel_gen_question
            )
            question_search = search_by_template(question_form)

            answer_form = ScTemplate()
            answer_form.quintuple(
                result_set[_],
                sc_types.EDGE_D_COMMON_VAR,
                sc_types.LINK_VAR,
                sc_types.EDGE_ACCESS_VAR_POS_PERM,
                nrel_corr_answer
            )
            answer_search = search_by_template(answer_form)

            question_str = get_link_content(question_search[0].get(2))
            answer_str = get_link_content(answer_search[0].get(2))

            question_obj = ScTestQuestionClass(question_str[0].data, answer_str[0].data,["","","",""])

            question_list.append(question_obj)
        bot_thread.join()
        self.logger.info("Bot finished by TG Agent")
        return ScResult.OK


def start_bot():
    while True:
        try:
            bot.polling(none_stop=True, interval=0.5)
        except Exception as e:
            print(f"Ошибка в боте: {e}")
            time.sleep(1)
            bot.stop_polling()
#todo: добавить итератор и пихать в словарь
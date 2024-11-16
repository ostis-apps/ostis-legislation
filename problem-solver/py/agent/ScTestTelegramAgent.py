import logging
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
from data import question_list, NUMBER_OF_QUESTIONS, result_set

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
        from modules.telegram_data import start_bot
        self.logger.info("TG Agent began to run")

        question_thread = threading.Thread(target=self.generate_questions)
        question_thread.start()

        question_thread.join()

        bot_thread = threading.Thread(target=start_bot)
        bot_thread.start()

        self.logger.info("Bot finished by TG Agent")
        return ScResult.OK

    def generate_questions(self):
        class_node = self.create_class_node()
        self.create_initial_relations(class_node)
        time.sleep(3)
        result_set = self.search_for_structured_nodes(class_node)

        for _ in range(NUMBER_OF_QUESTIONS):
            question_str, answer_str = self.retrieve_question_and_answer(result_set, _)
            question_obj = ScTestQuestionClass(question_str, answer_str, ["тут", "будет", "текст"])
            question_list.append(question_obj)

    def create_class_node(self) -> ScAddr:
        class_node = create_node(sc_types.NODE_VAR)
        return class_node

    def create_initial_relations(self, class_node):
        action_initiated = sc_kpm.ScKeynodes["action_initiated"]
        question_node = sc_kpm.ScKeynodes["action_generate_questions"]
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, question_node, class_node)

        utils.create_role_relation(class_node, ScKeynodes.get("question_model"), ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, action_initiated, class_node)

    def search_for_structured_nodes(self, class_node):
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

        result_numbered_set = ScNumberedSet(set_node=result_search)
        for value in result_numbered_set:
            result_set.append(value)
            if len(result_set) == 10:
                break
        return result_set

    def retrieve_question_and_answer(self, result_set, index):
        question_form = ScTemplate()
        question_form.quintuple(
            result_set[index],
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.LINK_VAR,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            sc_kpm.ScKeynodes["nrel_generated_question_text"]
        )
        question_search = search_by_template(question_form)

        answer_form = ScTemplate()
        answer_form.quintuple(
            result_set[index],
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.LINK_VAR,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            sc_kpm.ScKeynodes["nrel_generated_question_correct_answer"]
        )
        answer_search = search_by_template(answer_form)

        question_str = get_link_content(question_search[0].get(2))
        answer_str = get_link_content(answer_search[0].get(2))

        return question_str[0].data, answer_str[0].data


    def add_answer_relation(self, question_index: int, is_correct: bool):
        if question_index < len(result_set):
            question_node = result_set[question_index]

            if is_correct:
                logging.info(f"Вопрос {question_index}: Ответ правильный.")

                answer = ScTemplate()
                answer.quintuple(
                    question_node,
                    sc_types.EDGE_D_COMMON_VAR,
                    sc_types.LINK_VAR,
                    sc_types.EDGE_ACCESS_VAR_POS_PERM,
                    sc_kpm.ScKeynodes["nrel_generated_question_correct_answer"]
                )
                answer_search = search_by_template(answer)

                if answer_search:
                    binary_edge = answer_search[0].get(1)
                    create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM,
                                ScKeynodes.resolve("nrel_user_answer",
                                                   sc_types.NODE_CONST_NOROLE), binary_edge)
            else:
                logging.info(f"Вопрос {question_index}: Ответ неправильный.")
        else:
            logging.error(f"Invalid question index: {question_index}. It should be less than {len(result_set)}.")


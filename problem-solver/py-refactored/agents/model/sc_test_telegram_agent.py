import logging
from sc_kpm.identifiers import CommonIdentifiers
from agent_factory import load_agents
from agents.model.sc_test_question_class import ScTestQuestionClass
from sc_client.client import search_by_template, get_link_content
import time
import sc_kpm
from sc_client.models import ScAddr, ScTemplate, ScTemplateResult
from sc_client.constants import sc_types
from sc_kpm import ScAgentClassic, ScResult, ScKeynodes
from sc_kpm.sc_sets import ScNumberedSet, ScStructure
import threading
import sc_kpm.utils as utils
from sc_kpm.utils import create_node, create_edge, get_element_by_norole_relation, delete_edges
from model.telegram import run_bot, bot
from cfg import Config
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
        agents = load_agents()
        incorrect_answer_agent = agents.get("incorrect_answer_agent")
        question_agent = agents.get("question_agent")
        time.sleep(1)
        question_agent.execute()
        struct = get_question_agent_struct()
        time.sleep(1)
        incorrect_answer_agent.execute(struct)
        question_set = init_test_set(struct)
        time.sleep(1)
        questions = parse_all_questions(question_set)
        bot.set_questions(questions)

        bot_thread = threading.Thread(target=run_bot)
        bot_thread.start()
        self.logger.info("Bot finished by TG Agent")
        return ScResult.OK


def get_question_agent_struct() -> ScAddr:
    time.sleep(1)
    template = ScTemplate()
    template.triple(
        ScKeynodes.get("action_generate_questions"),
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        sc_types.NODE_VAR
    )
    result = search_by_template(template)
    time.sleep(1)
    node = result[0][2]
    template = ScTemplate()
    time.sleep(1)
    template.quintuple(
        node,
        sc_types.EDGE_D_COMMON_VAR,
        sc_types.NODE_VAR_STRUCT,
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        ScKeynodes.get("nrel_result")
    )
    time.sleep(1)
    result = search_by_template(template)
    time.sleep(1)
    return result[0][2]


def init_test_set(struct) -> list[ScAddr]:
    template = ScTemplate()
    template.triple(
        struct,
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        sc_types.NODE_VAR_TUPLE
    )
    result = search_by_template(template)
    result = result[0][2]
    numbered_set = ScNumberedSet(set_node=result)
    result_set = []
    for number in numbered_set:
        result_set.append(number)
        if len(result_set) == 10:
            break
    return result_set


def parse_all_questions(numbered_set: list[ScAddr]) -> list[ScTestQuestionClass]:
    questions = []
    for i in range(len(numbered_set)):
        question_text = parse_question([numbered_set[i]])
        correct_answer = parse_correct_answer([numbered_set[i]])
        incorrect_answers = parse_incorrect_answer([numbered_set[i]])
        question_obj = ScTestQuestionClass(
            question=question_text,
            correct_answer=correct_answer,
            incorrect_answers=incorrect_answers
        )
        questions.append(question_obj)
    return questions


def parse_incorrect_answer(numbered_set: list[ScAddr]) -> list[str]:
    incorrect_form = ScTemplate()
    incorrect_form.quintuple(
        numbered_set[0],
        sc_types.EDGE_D_COMMON_VAR,
        sc_types.LINK_VAR,
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        sc_kpm.ScKeynodes["nrel_generated_question_incorrect_answer"]
    )
    incorrect_search = search_by_template(incorrect_form)
    incorrect_str = [get_link_content(incorrect.get(2))[0].data for incorrect in incorrect_search]
    return incorrect_str


def parse_correct_answer(numbered_set: list[ScAddr]) -> str:
    template = ScTemplate()
    template.quintuple(
        numbered_set[0],
        sc_types.EDGE_D_COMMON_VAR,
        sc_types.LINK_VAR,
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        sc_kpm.ScKeynodes["nrel_generated_question_correct_answer"]
    )
    question = search_by_template(template)
    text = get_link_content(question[0].get(2))
    return text[0].data


def parse_question(numbered_set: list[ScAddr]) -> str:
    template = ScTemplate()
    template.quintuple(
        numbered_set[0],
        sc_types.EDGE_D_COMMON_VAR,
        sc_types.LINK_VAR,
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        sc_kpm.ScKeynodes["nrel_generated_question_text"]
    )
    question = search_by_template(template)
    text = get_link_content(question[0].get(2))
    return text[0].data


def add_pos_relation(question_index: int):
    struct = get_question_agent_struct()
    numbered_set = init_test_set(struct)
    if question_index < len(numbered_set):
        question_node = numbered_set[question_index]
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
            create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.resolve("nrel_user_answer", sc_types.NODE_CONST_NOROLE), binary_edge)
    else:
        logging.error(f"Invalid question index: {question_index}. It should be less than {len(numbered_set)}.")


def add_neg_relation(question_index: int):
    struct = get_question_agent_struct()
    numbered_set = init_test_set(struct)
    if question_index < len(numbered_set):
        question_node = numbered_set[question_index]
        answer = ScTemplate()
        answer.quintuple(
            question_node,
            sc_types.EDGE_D_COMMON_VAR,
            sc_types.LINK_VAR,
            sc_types.EDGE_ACCESS_VAR_POS_PERM,
            sc_kpm.ScKeynodes["nrel_generated_question_incorrect_answer"]
        )
        answer_search = search_by_template(answer)
        if answer_search:
            binary_edge = answer_search[0].get(1)
            create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.resolve("nrel_user_answer", sc_types.NODE_CONST_NOROLE), binary_edge)
    else:
        logging.error(f"Invalid question index: {question_index}. It should be less than {len(numbered_set)}.")
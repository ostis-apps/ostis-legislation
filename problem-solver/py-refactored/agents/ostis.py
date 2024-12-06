import threading
from enum import Enum
from threading import Event

import telebot
from sc_client.client import search_by_template

from cfg import Config
from sc_kpm.utils import create_node, create_nodes, create_edge
from sc_kpm.utils import create_binary_relation, create_role_relation, create_norole_relation

from agents.abstract.generate_incorrect_answer_agent import GenerateIncorrectAnswerAgent
from agents.abstract.generate_recommendations_agent import GenerateRecommendationsAgent
from agents.abstract.question_agent import GenerateQuestionAgent
from agents.abstract.telegram_session_agent import TelegramSessionAgent

import sc_client.client as client
from sc_client.constants import sc_types, sc_type
from sc_client.models import ScAddr, ScConstruction, ScTemplate, ScTemplateResult
from sc_kpm import ScKeynodes
from sc_kpm.sc_sets import ScStructure

payload = None
callback_event = Event()

class result(Enum):
    SUCCESS = 1
    FAIl = 0


class ScMachine:
    def __init__(self, url, tg = Config.BOT_TOKEN):
        self.platform = url
        self.telegram = tg

    def call_question_agent(self, question_model=ScKeynodes.get("question_model")):
        action_node = create_node(sc_types.NODE_CONST)
        create_role_relation(action_node, question_model, ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.get("action_generate_questions"), action_node)
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.get("action_initiated"), action_node)
        return result.SUCCESS


    def call_recommendation_agent(self, struct: ScStructure):
        action_node = create_node(sc_types.NODE_CONST)
        create_role_relation(action_node, struct, ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.get("action_generate_recommendations"),
                    action_node)
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.get("action_initiated"), action_node)
        return result.SUCCESS

    def call_incorrect_answer_agent(self, struct: ScStructure):
        action_node = create_node(sc_types.NODE_CONST)
        create_role_relation(action_node, struct, ScKeynodes.rrel_index(1))
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.get("action_generate_incorrect_answers"), action_node)
        create_edge(sc_types.EDGE_ACCESS_CONST_POS_PERM, ScKeynodes.get("action_initiated"), action_node)
        return result.SUCCESS

    def call_telegram_agent(self):
        pass

class ScGenerateIncorrectAnswerAgent(GenerateIncorrectAnswerAgent):
    def __init__(self):
        self.machine = ScMachine(Config.OSTIS_URL)

    def execute(self, struct: ScStructure):
        global payload
        payload = None
        return {"normalno": self.machine.call_incorrect_answer_agent(struct)}


class ScTelegramSessionAgent(TelegramSessionAgent):
    def __init__(self):
        self.machine = ScMachine(Config.OSTIS_URL)

    def execute(self):
        global payload
        payload = None
        return {"normalno": self.machine.call_telegram_agent()}


class ScGenerateRecommendationsAgent(GenerateRecommendationsAgent):
    def __init__(self):
        self.machine = ScMachine(Config.OSTIS_URL)

    def execute(self, struct: ScStructure):
        global payload
        payload = None
        return {"normalno": self.machine.call_recommendation_agent(struct)}


class ScGenerateQuestionAgent(GenerateQuestionAgent):
    def __init__(self):
        self.machine = ScMachine(Config.OSTIS_URL)

    def execute(self, question_model= ScKeynodes.get("question_model")):
        global payload
        payload = None
        return {"normalno": self.machine.call_question_agent()}


def get_node(client) -> ScAddr:
    construction = ScConstruction()
    construction.create_node(sc_types.NODE_CONST)
    main_node: ScAddr = client.create_elements(construction)[0]
    return main_node
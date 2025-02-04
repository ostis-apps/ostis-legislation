from sc_client.constants import sc_type
from sc_kpm import ScKeynodes
from sc_kpm.sc_sets import ScStructure

from .abstract.generate_incorrect_answer_agent import GenerateIncorrectAnswerAgent
from .abstract.generate_recommendations_agent import GenerateRecommendationsAgent
from .abstract.question_agent import GenerateQuestionAgent
from .abstract.telegram_session_agent import TelegramSessionAgent


class ScTelegramSessionAgent(TelegramSessionAgent):
    def execute(self):
        return {"normalno": "chetko"}

class ScGenerateIncorrectAnswerAgent(GenerateIncorrectAnswerAgent):
    def execute(self, struct: ScStructure):
        return {"normalno": "chetko"}


class ScGenerateRecommendationsAgent(GenerateRecommendationsAgent):
    def execute(self, struct: ScStructure):
        return {"normalno": "chetko"}


class ScGenerateQuestionAgent(GenerateQuestionAgent):
    def execute(self, question_model: ScKeynodes.resolve("question_model", sc_type.CONST_NODE_CLASS)):
        return {"normalno": "chetko"}

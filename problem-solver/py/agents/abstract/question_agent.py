from abc import ABC, abstractmethod
from sc_client.constants import sc_type
from sc_kpm import ScKeynodes


class GenerateQuestionAgent(ABC):
    @abstractmethod
    def execute(self, question_model: ScKeynodes.resolve("question_model", sc_type.CONST_NODE_CLASS)):
        pass
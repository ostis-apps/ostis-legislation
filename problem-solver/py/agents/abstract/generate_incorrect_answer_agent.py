from abc import ABC, abstractmethod

from sc_kpm.sc_sets import ScStructure


class GenerateIncorrectAnswerAgent(ABC):
    @abstractmethod
    def execute(self, struct: ScStructure):
        pass
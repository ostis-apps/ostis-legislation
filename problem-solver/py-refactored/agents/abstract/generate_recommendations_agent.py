from abc import ABC, abstractmethod

from sc_kpm.sc_sets import ScStructure


class GenerateRecommendationsAgent(ABC):
    @abstractmethod
    def execute(self, structure: ScStructure):
        pass
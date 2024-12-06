from abc import ABC, abstractmethod

class TelegramSessionAgent(ABC):
    @abstractmethod
    def execute(self):
        pass
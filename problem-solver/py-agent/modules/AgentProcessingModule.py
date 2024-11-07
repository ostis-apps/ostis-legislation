from sc_kpm import ScModule
from .ScTestTelegramAgent import TelegramScAgent


class AgentProcessingModule(ScModule):
    def __init__(self):
        super().__init__(TelegramScAgent())
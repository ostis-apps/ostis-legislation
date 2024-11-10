from sc_kpm import ScModule
from agent.ScTestTelegramAgent import TelegramScAgent

import logging
logging.basicConfig(level=logging.INFO)

class AgentProcessingModule(ScModule):
    def __init__(self):
        super().__init__(TelegramScAgent())
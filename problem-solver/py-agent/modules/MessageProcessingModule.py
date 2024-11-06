from sc_kpm import ScModule
from .ScTestTelegramAgent import TestScAgent


class MessageProcessingModule(ScModule):
    def __init__(self):
        super().__init__(TestScAgent())
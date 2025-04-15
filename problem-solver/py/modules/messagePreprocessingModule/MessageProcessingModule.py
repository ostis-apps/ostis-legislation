from sc_kpm import ScModule
from .TextUtilsAgent import TextUtilsAgent


class MessageProcessingModule(ScModule):
    def __init__(self):
        super().__init__(TextUtilsAgent())
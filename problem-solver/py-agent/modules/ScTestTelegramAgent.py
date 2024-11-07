import logging
from sc_client.models import ScAddr
from sc_kpm import ScAgentClassic, ScResult

logging.basicConfig(level=logging.INFO)

class TelegramScAgent(ScAgentClassic):
    def __init__(self):
        super().__init__("telegram_start_agent")

    def on_event(self, event_element: ScAddr, event_edge: ScAddr, action_element: ScAddr) -> ScResult:
        self.logger.info("TG Agent was called")
        result = self.__run()
        self.logger.info("TG Agent finished %s")
        return result

    def __run(self) -> ScResult:
        self.logger.info("TG Agent began to run")
        self.logger.info("Bot started by TG Agent")
        return ScResult.OK

import logging
from threading import Thread
from .telegram_data import start_bot
from sc_client.models import ScAddr
from sc_kpm import ScAgentClassic, ScResult
from sc_kpm.utils.action_utils import (
    finish_action_with_status,
)
logging.basicConfig(level=logging.INFO)


class TelegramScAgent(ScAgentClassic):
    def __init__(self):
        super().__init__("telegram_start_agent")

    def on_event(self, event_element: ScAddr, event_edge: ScAddr, action_element: ScAddr) -> ScResult:
        self.logger.info("TG Agent was called")
        result = self.run(action_element)
        is_successful = result == ScResult.OK
        finish_action_with_status(action_element, is_successful)
        self.logger.info("TG Agent finished %s", "successfully" if is_successful else "unsuccessfully")
        return result

    def run(self, action_node: ScAddr) -> ScResult:
        self.logger.info("TG Agent began to run")
        bot_thread = Thread(target = start_bot)
        bot_thread.start()
        self.logger.info("Bot started by TG Agent")
        return ScResult.OK

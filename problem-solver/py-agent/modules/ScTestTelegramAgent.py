import threading
import logging
from sc_kpm.identifiers import CommonIdentifiers
from sc_client.client import template_search, create_elements
from sc_client.client import create_elements, get_link_content
from sc_client.constants import sc_types
from sc_client.models import ScAddr, ScTemplateResult, ScTemplate,ScConstruction,ScLinkContent, ScLinkContentType
from sc_kpm.sc_sets import ScSet
from sc_client.constants import sc_types
from sc_kpm.utils import create_node, create_nodes, create_link, create_edge
from sc_client.constants.common import ScEventType
from sc_kpm import ScAgentClassic, ScModule, ScResult, ScServer, ScKeynodes
from sc_kpm.utils.action_utils import (
    create_action_answer,
    execute_agent,
    finish_action_with_status,
    get_action_answer,
    get_action_arguments, create_action,
)
logging.basicConfig(level=logging.INFO)
class TestScAgent(ScAgentClassic):
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
        self.logger.info("Bot started by TG Agent")
        return ScResult.OK

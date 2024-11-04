import threading
import logging
import telebot
from sc_client.client import template_search, create_elements
from telebot import types
from sc_client.client import create_elements, get_link_content
from sc_client.constants import sc_types
from sc_client.models import ScAddr, ScTemplateResult, ScTemplate,ScConstruction,ScLinkContent, ScLinkContentType
from sc_kpm.sc_sets import ScSet
from sc_client.constants import sc_types
from sc_kpm.utils import create_node, create_nodes
from sc_client.constants.common import ScEventType
from sc_kpm import ScAgentClassic, ScModule, ScResult, ScServer, ScKeynodes
from sc_kpm.utils.action_utils import (
    create_action_answer,
    execute_agent,
    finish_action_with_status,
    get_action_answer,
    get_action_arguments,
)
logging.basicConfig(level=logging.INFO)

bot = telebot.TeleBot('7779388088:AAEtaxwQcH43XNAuKuHLRFZsWDdtObTH__Q')


str_content = ScLinkContent("OSTIS LEGISLATION TEST AGENT", ScLinkContentType.STRING)
link_addr = ScAddr()
link_content = ScLinkContent(12, ScLinkContentType.STRING, link_addr)

@bot.message_handler(commands=['start'])
def start(message):
    markup = types.ReplyKeyboardMarkup(resize_keyboard=True)
    btn1 = types.KeyboardButton("👋 Привет! Я " + str_content.data)
    markup.add(btn1)
    bot.send_message(message.from_user.id, "👋 Привет! Я " + str_content.data, reply_markup=markup)

class TestScAgent(ScAgentClassic):
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


def start_bot():
    try:
        bot.polling(non_stop=True)
    except Exception as e:
        print("Не удалось запустить бота:", e)

def main():
    SC_SERVER_URL = "ws://localhost:8090/ws_json"
    server = ScServer(SC_SERVER_URL)
    with server.connect():
        action_class_name = "telegram_start"
        agent = TestScAgent(action_class_name, "ScEventType.ADD_OUTGOING_EDGE")
        module = ScModule(agent)
        server.add_modules(module)
        with server.register_modules():
            # Запуск потока для бота
            bot_thread = threading.Thread(target=start_bot)
            bot_thread.start()

            # Проверка, успешно ли запущен бот
            if bot_thread.is_alive():
                print("Бот успешно запущен.")
            else:
                print("Не удалось запустить бота.")

            # Выполнение действия агента
            action, is_successful = execute_agent(
                arguments={},
                concepts=[],
                initiation=action_class_name,
                wait_time=1,
            )
            # Поддерживаем основную программу активной, чтобы бот продолжал работать
            bot_thread.join()

main()

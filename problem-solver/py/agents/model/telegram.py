import random
import threading
from sc_client.client import search_by_template, get_link_content
from sc_client.constants import sc_types
import sc_kpm
import telebot
from sc_client.models import ScTemplate
from telebot import types
from cfg import Config
from model.proxy_clearing import ClearingProxy
from model.proxy_relation import RelationHandler
from model.proxy_recommendations import RecommendationsAgentProxy

shutdown_event = threading.Event()

class Telegram(telebot.TeleBot):
    def __init__(self, token):
        super().__init__(token)
        self.user_states = {}
        self.questions = []
        self.relation_handler = RelationHandler(self)
        self.recommendations_handler = RecommendationsAgentProxy(self)
        self.clearing = ClearingProxy()

    def set_questions(self, question_list):
        self.questions = question_list

    def start_bot(self, token=Config.BOT_TOKEN):
        while not shutdown_event.is_set():
            try:
                self.polling(none_stop=True, interval=1)
                print("2222222")
            except Exception as e:
                print(f"Ошибка в боте: {e}")

    def process_answer(self, current_index: int, is_correct: bool):
        if is_correct:
            self.relation_handler.add_pos_relation(current_index)
        else:
            self.relation_handler.add_neg_relation(current_index)


bot = Telegram(Config.BOT_TOKEN)

def run_bot():
    bot.start_bot()

@bot.message_handler(commands=['start'])
def start(message):
    user_id = message.from_user.id
    if user_id in bot.user_states:
        del bot.user_states[user_id]
    markup = types.InlineKeyboardMarkup()
    btn1 = types.InlineKeyboardButton("Начать тест!", callback_data='que_0')
    markup.add(btn1)
    bot.send_message(
        message.chat.id,
        "👋 Привет! Я бот для прохождения тестов по ИСС юриспруденции",
        reply_markup=markup
    )

@bot.message_handler(commands=['start'])
def start(message):
    if not bot.questions:
        bot.send_message(message.chat.id, "Вопросы не загружены. Выполните ребилд БЗ.")
        return
    markup = types.InlineKeyboardMarkup()
    btn1 = types.InlineKeyboardButton("Начать тест!", callback_data='que_0')
    markup.add(btn1)
    bot.send_message(
        message.chat.id,
        "👋 Привет! Я бот для прохождения тестов по ИСС юриспруденции",
        reply_markup=markup
    )


@bot.callback_query_handler(func=lambda call: True)
def callback(call):
    user_id = call.from_user.id
    try:
        if call.data == "que_0":
            bot.delete_message(call.message.chat.id, call.message.message_id)
            bot.user_states[user_id] = 0
            send_question(call.message, user_id)
        else:
            _, current_index, selected_index = call.data.split('_')
            current_index = int(current_index)
            selected_index = int(selected_index)

            question = bot.questions[current_index]
            correct_answer = question.correct_answer
            all_answers = question.incorrect_answers + [correct_answer]

            is_correct = all_answers[selected_index] == correct_answer
            bot.process_answer(current_index, is_correct)
            response_text = (
                f"✅ Правильно!\n\n{question.question}\n\nВаш ответ: {all_answers[selected_index]}"
                if is_correct
                else f"❌ Неправильно!\n\n{question.question}\n\nВаш ответ: {all_answers[selected_index]}"
            )

            bot.edit_message_text(
                chat_id=call.message.chat.id,
                message_id=call.message.message_id,
                text=response_text
            )
            bot.user_states[user_id] += 1
            if bot.user_states[user_id] < len(bot.questions):
                send_question(call.message, user_id)
            else:
                bot.recommendations_handler.execute()
                recommendations_message = get_recommendation_text()
                bot.send_message(call.message.chat.id, recommendations_message)

                bot.send_message(
                    call.message.chat.id,
                    "Тест завершён!",
                )
                del bot.user_states[user_id]
                bot.clearing.execute()
                bot.stop_polling()
                print("Goodbye!")

    except Exception as e:
        print(f"Ошибка обработки ответа: {e}")


def send_question(message, user_id):
    current_index = bot.user_states.get(user_id, 0)
    if current_index >= len(bot.questions):
        bot.send_message(message.chat.id, "Ошибка: больше вопросов нет.")
        return

    question = bot.questions[current_index]
    correct_answer = question.correct_answer
    all_answers = question.incorrect_answers + [correct_answer]
    shuffled_answers = list(enumerate(all_answers))
    random.shuffle(shuffled_answers)

    options_text = "\n".join(
        [f"{idx + 1}. {answer}" for idx, (_, answer) in enumerate(shuffled_answers)]
    )
    message_text = f"{question.question}\n\nВарианты ответа:\n{options_text}"

    markup = types.InlineKeyboardMarkup()
    for idx, (original_index, _) in enumerate(shuffled_answers):
        callback_data = f"answer_{current_index}_{original_index}"
        markup.add(types.InlineKeyboardButton(f"Вариант {idx + 1}", callback_data=callback_data))

    bot.send_message(message.chat.id, message_text, reply_markup=markup)


def get_recommendation_text() -> str:
    template = ScTemplate()
    template.quintuple(
        sc_kpm.ScKeynodes["test"],
        sc_types.EDGE_D_COMMON_VAR,
        sc_types.LINK_VAR,
        sc_types.EDGE_ACCESS_VAR_POS_PERM,
        sc_kpm.ScKeynodes["nrel_test_recommendations"]
    )
    result = search_by_template(template)
    result = get_link_content(result[0].get(2))
    message = result[0].data
    return message


import telebot
from telebot import types
import time
from ScTestQuestionClass import ScTestQuestionClass
from agent.ScTestTelegramAgent import question_list
import random

bot = telebot.TeleBot('7779388088:AAEtaxwQcH43XNAuKuHLRFZsWDdtObTH__Q')

class Telegram:
    def __init__(self, sc_tg_question: list[ScTestQuestionClass]):
        self.__sc_tg_question = sc_tg_question

    @property
    def sc_tg_question(self) -> list[ScTestQuestionClass]:
        return self.__sc_tg_question

    @sc_tg_question.setter
    def sc_tg_question(self, value: list[ScTestQuestionClass]):
        self.__sc_tg_question = value

    def get_question_by_text(self, text: str) -> ScTestQuestionClass:
        for question in self.__sc_tg_question:
            if question.question == text:
                return question
        raise ValueError("Вопрос с таким текстом не найден.")

    def get_correct_answer(self, question_text: str) -> str:
        question = self.get_question_by_text(question_text)
        return question.correct_answer

    def get_incorrect_answers(self, question_text: str) -> list[str]:
        question = self.get_question_by_text(question_text)
        return question.incorrect_answers


@bot.message_handler(commands=['start'])
def start(message):
    markup = types.InlineKeyboardMarkup()
    btn1 = types.InlineKeyboardButton("Начать тест!", callback_data='que_0')
    markup.add(btn1)
    bot.send_message(
        message.chat.id,
        "👋 Привет! Я бот для прохождения тестов по ИСС юриспруденции",
        reply_markup=markup
    )

telegram_questions = Telegram(sc_tg_question=question_list)

user_states = {}

@bot.callback_query_handler(func=lambda call: True)
def callback(call):
    user_id = call.from_user.id

    if call.data == "que_0":
        user_states[user_id] = 0
        send_question(call.message, user_id)
    else:

        current_index = user_states.get(user_id, 0)
        question = telegram_questions.sc_tg_question[current_index]
        correct_answer = question.correct_answer

        if call.data == correct_answer:
            bot.answer_callback_query(call.id, "✅ Правильно!")
        else:
            bot.answer_callback_query(call.id, "❌ Неправильно!")

        user_states[user_id] += 1
        if user_states[user_id] < len(telegram_questions.sc_tg_question):
            send_question(call.message, user_id)
        else:
            bot.send_message(call.message.chat.id, "🎉 Тест завершен! Спасибо за участие.")
            del user_states[user_id]


def send_question(message, user_id):
    current_index = user_states[user_id]
    question = telegram_questions.sc_tg_question[current_index]

    markup = types.InlineKeyboardMarkup()
    answers = question.incorrect_answers + [question.correct_answer]
    random.shuffle(answers)

    for answer in answers:
        markup.add(types.InlineKeyboardButton(answer, callback_data=answer))

    bot.send_message(
        message.chat.id,
        question.question,
        reply_markup=markup
    )


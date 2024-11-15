import telebot
from telebot import types
from ScTestQuestionClass import ScTestQuestionClass
from data import question_list
import random
from data import bot


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
    if not question_list:
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


user_states = {}
telegram_questions = Telegram(sc_tg_question=question_list)
print(f"Вопросы: {question_list}")
print(f"Количество вопросов: {len(question_list)}")


@bot.callback_query_handler(func=lambda call: True)
def callback(call):
    user_id = call.from_user.id

    if call.data == "que_0":
        user_states[user_id] = 0
        send_question(call.message, user_id)
    else:
        _, current_index, original_index = call.data.split('_')
        current_index = int(current_index)
        original_index = int(original_index)

        question = telegram_questions.sc_tg_question[current_index]
        correct_answer = question.correct_answer
        answers = question.incorrect_answers + [correct_answer]

        # After shuffling, we know the correct answer index is the same as original_index
        shuffled_answers = random.sample(answers, len(answers))
        selected_answer = shuffled_answers[original_index]

        # Debugging output
        print(
            f"Пользователь: {user_id}, Вопрос: {question.question}, Правильный ответ: {correct_answer}, Выбранный ответ: {selected_answer}"
        )

        if selected_answer == correct_answer:
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
    current_index = user_states.get(user_id, 0)
    if current_index >= len(telegram_questions.sc_tg_question):
        bot.send_message(message.chat.id, "Ошибка: больше вопросов нет.")
        return

    question = telegram_questions.sc_tg_question[current_index]
    correct_answer = question.correct_answer
    answers = question.incorrect_answers + [correct_answer]

    # Create a list of tuples (answer, original_index)
    answers_with_index = [(answer, idx) for idx, answer in enumerate(answers)]

    # Shuffle answers with their indices
    random.shuffle(answers_with_index)

    # Create the markup and encode the shuffled answer index in the callback_data
    markup = types.InlineKeyboardMarkup()
    for i, (answer, original_index) in enumerate(answers_with_index):
        callback_data = f"answer_{current_index}_{original_index}"  # Use the original index here
        markup.add(types.InlineKeyboardButton(f"Вариант {i + 1}", callback_data=callback_data))

    # Prepare the answer text for display
    answers_text = "\n\n".join([f"Вариант {i + 1}: {answer}" for i, (answer, _) in enumerate(answers_with_index)])
    try:
        bot.send_message(
            message.chat.id,
            f"{question.question}\n\n{answers_text}",
            reply_markup=markup
        )
    except Exception as e:
        print(f"Ошибка при отправке вопроса: {e}")


def start_bot():
    while True:
        try:
            bot.polling(none_stop=True, interval=0.5)
        except Exception as e:
            print(f"Ошибка в боте: {e}")
    print("Bot finished")
    bot.stop_polling()


#todo: намутить фикс правильных ответов и вопросов
#todo: добавить удаление кнопок и приветственного сообщения
import telebot
from telebot import types
import time
#from shutdown_manager import shutdown_manager

bot = telebot.TeleBot('7779388088:AAEtaxwQcH43XNAuKuHLRFZsWDdtObTH__Q')

questions = [
    {"text": "Вопрос 1: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 2: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 3: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 4: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 5: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 6: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 7: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 8: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 9: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]},
    {"text": "Вопрос 10: ", "options": ["Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4", "Ответ 5"]}
]

user_data = {}


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


@bot.callback_query_handler(func=lambda call: True)
def callback(call):
    if call.data.startswith("que_"):
        question_index = int(call.data.split("_")[1])

        if question_index < len(questions):
            # Получаем текущий вопрос и его варианты
            question = questions[question_index]
            question_text = question["text"]
            options = question["options"]

            # Создаем клавиатуру с вариантами ответа
            markup = types.InlineKeyboardMarkup()
            for i, option in enumerate(options):
                btn = types.InlineKeyboardButton(option, callback_data=f'ans_{question_index}_{i}')
                markup.add(btn)

            # Отправляем вопрос и кнопки
            bot.send_message(call.message.chat.id, question_text, reply_markup=markup)
        else:
            # Если все вопросы завершены
            bot.send_message(call.message.chat.id, "Тест завершен. Спасибо за участие!")

    elif call.data.startswith("ans_"):
        # Обрабатываем ответ на текущий вопрос
        _, question_index, answer_index = call.data.split("_")
        question_index = int(question_index)
        answer_index = int(answer_index)

        # Сохраняем ответ пользователя
        user_id = call.message.chat.id
        if user_id not in user_data:
            user_data[user_id] = []
        user_data[user_id].append((question_index, answer_index))

        # Получаем текст выбранного варианта
        selected_option = questions[question_index]["options"][answer_index]

        # Отредактируем сообщение с подтверждением ответа
        answer_confirmation_text = f"{call.message.text}\nОтвет записан - ваш ответ: {selected_option}"

        # Убираем кнопки (редактируем сообщение)
        bot.edit_message_reply_markup(call.message.chat.id, call.message.message_id, reply_markup=None)

        # Редактируем текст в сообщении, добавляем информацию о выбранном ответе
        bot.edit_message_text(answer_confirmation_text, call.message.chat.id, call.message.message_id)

        # Переход к следующему вопросу
        next_question_index = question_index + 1
        if next_question_index < len(questions):
            # Отправляем следующий вопрос с новыми кнопками
            question = questions[next_question_index]
            question_text = question["text"]
            options = question["options"]

            # Создаем клавиатуру с вариантами ответа
            markup = types.InlineKeyboardMarkup()
            for i, option in enumerate(options):
                btn = types.InlineKeyboardButton(option, callback_data=f'ans_{next_question_index}_{i}')
                markup.add(btn)

            # Отправляем следующий вопрос
            bot.send_message(call.message.chat.id, question_text, reply_markup=markup)
        else:
            # Если все вопросы завершены
            bot.send_message(call.message.chat.id, "Тест завершен. Спасибо за участие!")


def start_bot():
    while True:
        try:
            bot.polling(none_stop=True, interval=0.5)
        except Exception as e:
            print(f"Ошибка в боте: {e}")
            time.sleep(1)
    print("Бот завершает работу.")
    bot.stop_polling()
import telebot
from telebot import types
import time
from shutdown_manager import shutdown_manager

bot = telebot.TeleBot('7779388088:AAEtaxwQcH43XNAuKuHLRFZsWDdtObTH__Q')

@bot.message_handler(commands=['start'])
def start(message):
    markup = types. InlineKeyboardMarkup()
    btn1 = types.InlineKeyboardButton("Начать тест!", callback_data='que')
    markup.add(btn1)
    bot.send_message(message.chat.id, "👋 Привет! Я бот для прохождения тестов по ИСС юриспруденции", reply_markup=markup)


@bot.callback_query_handler(func=lambda call: True)
def callback(call):
    if call.message:
        if call.data == 'que':
            message = bot.send_message(call.message.chat.id, 'Nachnem')
            time.sleep(1)
            bot.delete_message(call.message.chat.id, message.message_id)


def start_bot():
    while not shutdown_manager.is_stopped():
        try:
            bot.polling(none_stop=True, interval=0.5)
        except Exception as e:
            time.sleep(1)
    bot.stop_polling()
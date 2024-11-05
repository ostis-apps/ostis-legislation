import telebot
from telebot import types


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
            bot.send_message(call.message.chat.id,'Nachnem')


def start_bot():
    try:
        bot.polling(non_stop=True)
    except Exception as e:
        print("Не удалось запустить бота:", e)
#!/usr/bin/env python3
import smtplib
import sys
import os
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import logging
import ssl

# Настройка логирования
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('/tmp/email_debug.log'),
        logging.StreamHandler()
    ]
)

def send_token(email, token):
    try:
        # Параметры SMTP
        SMTP_SERVER = 'smtp.gmail.com'
        SMTP_PORT = 587
        SMTP_USER = 'urisprudenciiassistent@gmail.com'
        SMTP_PASSWORD = 'pwtnnifufwqlxmce'
        
        logging.info(f"Попытка отправки email на {email} с токеном {token}")
        
        # Создаем сообщение
        msg = MIMEMultipart()
        msg['From'] = SMTP_USER
        msg['To'] = email
        msg['Subject'] = 'Код подтверждения для Юридического форума'
        
        # Текст письма
        body = f"""
        Здравствуйте!
        
        Ваш код подтверждения для регистрации на Юридическом форуме: {token}
        
        Введите этот код в форме подтверждения для завершения регистрации.
        
        Если вы не регистрировались на нашем форуме, просто проигнорируйте это письмо.
        
        С уважением,
        Команда Юридического форума
        """
        
        msg.attach(MIMEText(body, 'plain'))
        
        # Отправка
        logging.info(f"Подключение к SMTP серверу {SMTP_SERVER}:{SMTP_PORT}")
        
        # Пробуем разные способы отправки
        try:
            # Способ 1: STARTTLS
            server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT, timeout=30)
            server.set_debuglevel(1)  # Включаем детальное логирование SMTP
            
            logging.info("Запуск STARTTLS")
            server.starttls(context=ssl.create_default_context())
            
            logging.info(f"Логин в {SMTP_USER}")
            server.login(SMTP_USER, SMTP_PASSWORD)
            
            logging.info(f"Отправка письма на {email}")
            text = msg.as_string()
            server.sendmail(SMTP_USER, email, text)
            
            logging.info("Завершение сессии")
            server.quit()
            
        except Exception as e1:
            logging.warning(f"Способ 1 не сработал: {e1}")
            
            try:
                # Способ 2: SSL с портом 465
                logging.info("Пробуем порт 465 с SSL")
                context = ssl.create_default_context()
                server = smtplib.SMTP_SSL(SMTP_SERVER, 465, context=context, timeout=30)
                server.set_debuglevel(1)
                
                logging.info(f"Логин в {SMTP_USER} через SSL")
                server.login(SMTP_USER, SMTP_PASSWORD)
                
                logging.info(f"Отправка письма на {email}")
                text = msg.as_string()
                server.sendmail(SMTP_USER, email, text)
                
                logging.info("Завершение сессии")
                server.quit()
                
            except Exception as e2:
                logging.error(f"Способ 2 также не сработал: {e2}")
                return False
        
        logging.info(f"Письмо успешно отправлено на {email}")
        return True
        
    except smtplib.SMTPAuthenticationError as e:
        logging.error(f"Ошибка аутентификации SMTP: {e}")
        logging.error("Проверьте логин и пароль. Возможно, нужно включить 'Ненадежные приложения' в настройках Google аккаунта.")
        return False
    except smtplib.SMTPException as e:
        logging.error(f"Ошибка SMTP: {e}")
        return False
    except Exception as e:
        logging.error(f"Общая ошибка: {e}")
        logging.error(f"Тип ошибки: {type(e).__name__}")
        return False

if __name__ == '__main__':
    logging.info("Скрипт send_token.py запущен")
    logging.info(f"Аргументы: {sys.argv}")
    logging.info(f"Текущая директория: {os.getcwd()}")
    
    if len(sys.argv) != 3:
        logging.error("Неверное количество аргументов. Использование: python3 send_token.py <email> <token>")
        print("ERROR: Invalid arguments")
        sys.exit(1)
    
    email = sys.argv[1]
    token = sys.argv[2]
    
    logging.info(f"Обработка запроса: email={email}, token={token}")
    
    success = send_token(email, token)
    
    if success:
        logging.info("Успешное завершение скрипта")
        print("SUCCESS")
        sys.exit(0)
    else:
        logging.error("Неудачное завершение скрипта")
        print("ERROR")
        sys.exit(1)

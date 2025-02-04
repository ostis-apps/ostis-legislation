import configparser


class Config:
    config = configparser.ConfigParser()
    config.read('cfg.ini')

    AGENTS_TO_LOAD = {
        "incorrect_answer_agent": "agents.ostis.ScGenerateIncorrectAnswerAgent",
        "question_agent": "agents.ostis.ScGenerateQuestionAgent",
        "recommendations_agent": "agents.ostis.ScGenerateRecommendationsAgent",
        "telegram_session_agent": "agents.ostis.ScTelegramSessionAgent"
    }
    OSTIS_URL = config['DEFAULT']['ostis_url']
    BOT_TOKEN = "7779388088:AAEtaxwQcH43XNAuKuHLRFZsWDdtObTH__Q"
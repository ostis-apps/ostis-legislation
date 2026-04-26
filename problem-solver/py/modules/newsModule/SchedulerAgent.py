import threading
import time
import logging

logger = logging.getLogger(__name__)


class SchedulerAgent:
    """
    Планировщик запуска цепочки парсинг → фильтрация.
    Обычный Python-класс (не ScAgent) — управляет потоками напрямую.
    """

    def __init__(self, parser_agent, filter_agent):
        self.parser = parser_agent
        self.filter = filter_agent
        self.running = True
        self.update_interval = 3600  # секунд между обновлениями

        logger.info("SchedulerAgent инициализирован")
        # Первый запуск через 5 секунд после старта
        threading.Timer(5.0, self._run_chain).start()
        self._start_timer_thread()

    def _start_timer_thread(self):
        def loop():
            while self.running:
                time.sleep(self.update_interval)
                if self.running:
                    self._run_chain()

        thread = threading.Thread(target=loop, daemon=True)
        thread.start()

    def _run_chain(self):
        logger.info("Запуск цепочки: парсинг -> фильтрация")
        try:
            raw_news = self.parser.fetch_raw_news()
            if not raw_news:
                logger.info("Нет новостей для обработки")
                return
            stats = self.filter.process_raw_news(raw_news)
            logger.info(
                f"Результат: всего={stats['total']}, "
                f"полезных={stats['useful']}, сохранено={stats['saved']}"
            )
        except Exception as e:
            logger.error(f"Ошибка в цепочке: {e}")

    def shutdown(self):
        self.running = False
        logger.info("SchedulerAgent остановлен")

import logging
from datetime import datetime
import cloudscraper
import feedparser

from sc_client.models import ScAddr
from sc_kpm import ScAgentClassic, ScResult


logger = logging.getLogger(__name__)

RSS_FEEDS = {
    'Общественно-политические новости': 'http://pravo.by/novosti/obshchestvenno-politicheskie-i-v-oblasti-prava/rss/',
    'Новости PRAVO.BY': 'http://pravo.by/novosti/novosti-pravo-by/rss/',
    'Национальный реестр правовых актов': 'http://www.pravo.by/novosti/rss-lenty/rss-reestr/',
    'Опубликовано на портале': 'http://www.pravo.by/novosti/rss-lenty/rss-portal/'
}

class RssParserAgent(ScAgentClassic):
    """
    Агент для парсинга RSS-лент (без сохранения в БД).
    Возвращает список словарей с сырыми новостями.
    """
    def __init__(self):
        super().__init__("action_update_rss")
        logger.info("RssParserAgent инициализирован")

    def on_event(self, class_node: ScAddr, edge: ScAddr, action_node: ScAddr) -> ScResult:
        # Этот метод может использоваться для стандартного вызова,
        # но мы будем использовать прямой вызов fetch_raw_news()
        logger.info("RssParserAgent: получено событие (игнорируется, используйте прямой вызов)")
        return ScResult.OK

    def fetch_raw_news(self):
        """
        Парсит все RSS-ленты и возвращает список словарей с сырыми новостями.
        """
        all_news = []
        for feed_name, feed_url in RSS_FEEDS.items():
            logger.info(f"Парсинг ленты: {feed_name}")
            entries = self._fetch_feed(feed_name, feed_url)
            all_news.extend(entries)
        logger.info(f"Всего сырых новостей: {len(all_news)}")
        return all_news

    def _fetch_feed(self, feed_name, feed_url):
        scraper = cloudscraper.create_scraper(
            browser={'browser': 'chrome', 'platform': 'windows', 'desktop': True, 'mobile': False}
        )
        headers = {
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Accept-Encoding': 'gzip, deflate, br',
            'Accept-Language': 'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7',
        }
        try:
            response = scraper.get(feed_url, headers=headers, timeout=30)
            if response.status_code != 200:
                logger.error(f"Ошибка {response.status_code} для {feed_name}")
                return []
            feed = feedparser.parse(response.text)
            news_list = []
            for entry in feed.entries:
                news = {
                    'title': entry.get('title', 'Без заголовка'),
                    'link': entry.get('link', ''),
                    'summary': entry.get('summary', entry.get('description', '')),
                    'published_date': self._parse_date(entry.get('published_parsed')),
                    'feed_source': feed_name,
                    'guid': entry.get('id', entry.get('link', '')),
                }
                if news['link']:
                    news_list.append(news)
            logger.info(f"{feed_name}: найдено {len(news_list)} записей")
            return news_list
        except Exception as e:
            logger.error(f"Ошибка парсинга {feed_name}: {e}")
            return []

    @staticmethod
    def _parse_date(date_struct):
        if date_struct:
            return datetime(*date_struct[:6])
        return datetime.now()
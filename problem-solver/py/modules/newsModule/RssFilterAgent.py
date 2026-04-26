# problem-solver/py/agents/rss_filter_agent.py
"""
Агент-фильтр для RSS новостей с поддержкой стемминга, белого списка источников,
негативных ключевых слов и усиления действий.
"""

import logging
from datetime import datetime
from pathlib import Path

import snowballstemmer
from sqlalchemy import create_engine, Column, String, DateTime, Text, Integer, func
from sqlalchemy.orm import declarative_base, sessionmaker

from sc_client.models import ScAddr
from sc_kpm import ScAgentClassic, ScResult

logger = logging.getLogger(__name__)

# ---------- Конфигурация путей и БД ----------
PROJECT_ROOT = Path(__file__).parent.parent.parent.parent.parent
DATA_DIR = PROJECT_ROOT / 'databases'
DATA_DIR.mkdir(parents=True, exist_ok=True)
DB_PATH = DATA_DIR / 'filtered_news.db'
DATABASE_URL = f"sqlite:///{DB_PATH}"

# ---------- Модель данных ----------
Base = declarative_base()

class FilteredNews(Base):
    __tablename__ = 'filtered_news'
    
    id = Column(Integer, primary_key=True)
    title = Column(String(500), nullable=False)
    link = Column(String(500), unique=True, nullable=False, index=True)
    summary = Column(Text)
    published_date = Column(DateTime, index=True)
    feed_source = Column(String(200), index=True)
    relevance_score = Column(Integer, default=0)
    matched_keywords = Column(String(500))
    created_at = Column(DateTime, default=datetime.now)

    def to_dict(self):
        return {
            'id': self.id,
            'title': self.title,
            'link': self.link,
            'summary': self.summary,
            'published_date': self.published_date.isoformat() if self.published_date else None,
            'feed_source': self.feed_source,
            'relevance_score': self.relevance_score,
            'matched_keywords': self.matched_keywords
        }

# ---------- Конфигурация фильтрации ----------
class FilterConfig:
    # Ключевые слова с положительными весами
    KEYWORDS = {
        'закон': 5,
        'постановление': 5,
        'указ': 5,
        'декрет': 5,
        'кодекс': 5,
        'законопроект': 4,
        'нормативный': 3,
        'правовой': 3,
        'юридический': 2,
        'вступает в силу': 4,
        'изменение': 3,
        'дополнение': 3,
        'поправка': 3,
        'президент': 4,
        'правительство': 4,
        'совет министров': 4,
        'парламент': 3,
        'депутат': 2,
        'налог': 3,
        'льгота': 3,
        'пенсия': 3,
        'субсидия': 3,
        'ответственность': 2,
        'штраф': 2,
    }
    
    # Ключевые слова с отрицательными весами (уменьшают релевантность)
    NEGATIVE_KEYWORDS = {
        'олимпиада': -5,
        'студент': -3,
        'конкурс': -3,
        'интервью': -3,
        'муткорт': -4,
        'хобби': -1,
        'поздравление': -2,
        'юбилей': -2,
        'выставка': -2,
        'концерт': -2,
        'спорт': -2,
        'футбол': -2,
        'хоккей': -2,
        'театр': -2,
        'кино': -2,
        'шоу': -2,
    }
    
    # Сильные действия — при их наличии добавляется бонус
    STRONG_ACTION_KEYWORDS = {
        'вступает в силу',
        'опубликован',
        'принят',
        'утвержден',
        'изменение',
        'дополнение',
        'поправка',
    }
    
    # Бонусный вес за наличие сильного действия
    STRONG_ACTION_BONUS = 5
    
    # Минимальный вес для сохранения новости
    MIN_SCORE = 1
    
    # Критические ключевые слова — при их наличии новость точно сохраняется
    CRITICAL_KEYWORDS = {'закон', 'указ', 'декрет', 'кодекс', 'постановление'}
    
    # Белый список источников (ленты, которые не требуют фильтрации)
    WHITELIST_SOURCES = {
        'Национальный реестр правовых актов',
        'Опубликовано на портале'
    }

# ---------- Агент-фильтр ----------
class RssFilterAgent(ScAgentClassic):
    """
    Агент для фильтрации и сохранения новостей в БД.
    Реагирует на действие "action_filter_rss".
    """
    def __init__(self):
        super().__init__("action_filter_rss")
        
        # Инициализация стеммера для русского языка
        self.stemmer = snowballstemmer.stemmer('russian')
        
        # Инициализация БД
        self._init_database()
        logger.info(f"RssFilterAgent инициализирован. БД: {DB_PATH}")
    
    def _init_database(self):
        """Создаёт таблицы в БД, если их нет"""
        self.engine = create_engine(DATABASE_URL)
        Base.metadata.create_all(self.engine)
        self.Session = sessionmaker(bind=self.engine)
    
    def on_event(self, class_node: ScAddr, edge: ScAddr, action_node: ScAddr) -> ScResult:
        """
        Вызывается при стандартной инициации действия action_filter_rss.
        (Может использоваться для ручного запуска фильтрации)
        """
        logger.info("RssFilterAgent: получено событие action_filter_rss")
        return ScResult.OK
    
    def process_raw_news(self, raw_news_list):
        """
        Принимает список сырых новостей (словарей), фильтрует и сохраняет в БД.
        Возвращает статистику обработки.
        """
        logger.info(f"Начало фильтрации {len(raw_news_list)} новостей")
        session = self.Session()
        
        try:
            useful_news = []
            for news in raw_news_list:
                analysis = self._analyze(news)
                if analysis['is_useful']:
                    news['analysis'] = analysis
                    useful_news.append(news)
                    logger.debug(f"[ПОЛЕЗНО] {news['title'][:50]} (вес: {analysis['score']})")
                else:
                    logger.debug(f"[ОТБРОШЕНО] {news['title'][:50]}")
            
            saved_count, skipped = self._save_to_db(session, useful_news)
            session.commit()

            if saved_count > 0:
                try:
                    from .NotificationAgent import NotificationAgent
                    notification_agent = NotificationAgent()
                    
                    # Получаем только что сохранённые новости для уведомления
                    new_news_for_notification = []
                    for news in useful_news:
                        if news.get('analysis', {}).get('is_useful'):
                            new_news_for_notification.append({
                                'id': news.get('link', ''),  # используем link как идентификатор
                                'title': news.get('title', ''),
                                'link': news.get('link', '')
                            })
                    
                    # Вызываем агент уведомлений
                    notified = notification_agent.add_notifications(new_news_for_notification)
                    logger.info(f"Создано уведомлений: {notified}")
                except Exception as e:
                    logger.error(f"Ошибка вызова агента уведомлений: {e}")
            
            result = {
                'total': len(raw_news_list),
                'useful': len(useful_news),
                'saved': saved_count,
                'filter_ratio': round(len(useful_news) / len(raw_news_list) * 100, 1) if raw_news_list else 0
            }
            logger.info(f"Фильтрация завершена: всего={result['total']}, полезных={result['useful']}, сохранено={result['saved']}")
            return result
            
        except Exception as e:
            session.rollback()
            logger.error(f"Ошибка при фильтрации: {e}", exc_info=True)
            raise
        finally:
            session.close()
    
    def _analyze(self, news):
        """
        Анализ релевантности новости с учётом положительных, отрицательных слов и сильных действий.
        """
        feed_source = news.get('feed_source', '')
        
        # Если источник в белом списке — сразу считаем полезным
        if feed_source in FilterConfig.WHITELIST_SOURCES:
            return {
                'score': 100,
                'is_useful': True,
                'matched_keywords': [f"WHITELIST:{feed_source}"]
            }
        
        # Формируем текст для анализа
        text = f"{news.get('title', '')} {news.get('summary', '')}".lower()
        words = text.split()
        stemmed_text = ' '.join(self.stemmer.stemWords(words))
        
        score = 0
        matched = []
        
        # 1. Проверка критических ключевых слов
        for kw in FilterConfig.CRITICAL_KEYWORDS:
            stemmed_kw = self.stemmer.stemWord(kw)
            if stemmed_kw in stemmed_text:
                score += 10
                matched.append(f"CRIT:{kw}")
        
        # 2. Положительные ключевые слова
        for kw, weight in FilterConfig.KEYWORDS.items():
            stemmed_kw = self.stemmer.stemWord(kw)
            if stemmed_kw in stemmed_text:
                score += weight
                if kw not in matched:
                    matched.append(kw)
        
        # 3. Отрицательные ключевые слова (штраф)
        for kw, penalty in FilterConfig.NEGATIVE_KEYWORDS.items():
            stemmed_kw = self.stemmer.stemWord(kw)
            if stemmed_kw in stemmed_text:
                score += penalty  # penalty отрицательный
                matched.append(f"NEG:{kw}")
        
        # 4. Бонус за сильные действия
        has_strong_action = False
        for action in FilterConfig.STRONG_ACTION_KEYWORDS:
            stemmed_action = self.stemmer.stemWord(action)
            if stemmed_action in stemmed_text:
                has_strong_action = True
                matched.append(f"ACTION:{action}")
        if has_strong_action:
            score += FilterConfig.STRONG_ACTION_BONUS
        
        is_useful = score >= FilterConfig.MIN_SCORE
        
        return {
            'score': score,
            'is_useful': is_useful,
            'matched_keywords': matched[:15]  # ограничиваем длину
        }
    
    def _save_to_db(self, session, news_list):
        """
        Сохраняет отфильтрованные новости в БД, пропуская дубликаты по ссылке.
        Возвращает кортеж (количество добавленных, количество пропущенных дубликатов).
        """
        saved = 0
        skipped = 0
        for news in news_list:
            link = news.get('link')
            if not link:
                continue
            
            exists = session.query(FilteredNews).filter_by(link=link).first()
            if exists:
                skipped += 1
                continue
            
            analysis = news.get('analysis', {})
            
            item = FilteredNews(
                title=news['title'],
                link=link,
                summary=news.get('summary', '')[:1000],
                published_date=news.get('published_date'),
                feed_source=news.get('feed_source'),
                relevance_score=analysis.get('score', 0),
                matched_keywords=','.join(analysis.get('matched_keywords', []))
            )
            session.add(item)
            saved += 1
            logger.debug(f"[СОХРАНЕНО] {news['title'][:60]}")
        
        return saved, skipped
    
    # ---------- Вспомогательные методы ----------
    def get_useful_news(self, limit=None):
        """Получить сохранённые новости из БД"""
        session = self.Session()
        try:
            query = session.query(FilteredNews).order_by(FilteredNews.published_date.desc())
            if limit:
                query = query.limit(limit)
            return [n.to_dict() for n in query.all()]
        finally:
            session.close()
    
    def get_statistics(self):
        """Получить статистику по отфильтрованным новостям"""
        session = self.Session()
        try:
            total = session.query(FilteredNews).count()
            by_source = session.query(
                FilteredNews.feed_source,
                func.count(FilteredNews.id)
            ).group_by(FilteredNews.feed_source).all()
            
            return {
                'total': total,
                'by_source': dict(by_source),
                'db_path': str(DB_PATH)
            }
        finally:
            session.close()
    
    def clear_database(self):
        """Очистить БД (только для тестирования)"""
        session = self.Session()
        try:
            count = session.query(FilteredNews).delete()
            session.commit()
            logger.warning(f"БД очищена. Удалено записей: {count}")
            return count
        finally:
            session.close()
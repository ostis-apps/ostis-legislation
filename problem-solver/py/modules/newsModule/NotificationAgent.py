# problem-solver/py/agents/notification_agent.py
"""
Агент-уведомитель для работы с уведомлениями о новых новостях.
Хранит информацию о непрочитанных новостях в SC-памяти.
"""

import logging
from datetime import datetime
from typing import List, Dict, Any
from pathlib import Path

from sc_client import client
from sc_client.constants import sc_types
from sc_client.models import ScAddr, ScConstruction, ScTemplate, ScIdtfResolveParams
from sc_kpm import ScAgentClassic, ScResult, ScKeynodes


logger = logging.getLogger(__name__)


class NotificationAgent(ScAgentClassic):
    """
    Агент для управления уведомлениями о новых новостях.
    Реагирует на действие "action_notify_news".
    """
    
    def __init__(self):
        super().__init__("action_notify_news")
        self._ensure_keynodes()
        logger.info("NotificationAgent инициализирован")
    
    def _ensure_keynodes(self):
        """Создаёт необходимые ключевые узлы в SC-памяти"""
        # Класс для хранения уведомлений
        ScKeynodes.resolve("notification_storage", sc_types.NODE_CONST_CLASS)
        # Класс для отдельного уведомления
        ScKeynodes.resolve("news_notification", sc_types.NODE_CONST_CLASS)
        # Отношение "не прочитано"
        ScKeynodes.resolve("nrel_unread", sc_types.NODE_CONST_NOROLE)
        # Отношение "время создания"
        ScKeynodes.resolve("nrel_created_at", sc_types.NODE_CONST_NOROLE)
        # Отношение "привязано к новости"
        ScKeynodes.resolve("nrel_news_link", sc_types.NODE_CONST_NOROLE)
        logger.info("Ключевые узлы для уведомлений созданы")
    
    def on_event(self, class_node: ScAddr, edge: ScAddr, action_node: ScAddr) -> ScResult:
        """Вызывается при инициации действия action_notify_news"""
        logger.info("NotificationAgent: получено событие action_notify_news")
        return ScResult.OK
    
    def add_notifications(self, news_list: List[Dict[str, Any]]) -> int:
        """
        Добавляет уведомления о новых новостях в SC-память.
        
        Args:
            news_list: список словарей с новостями (title, link, id)
        
        Returns:
            количество добавленных уведомлений
        """
        if not news_list:
            return 0
        
        try:
            storage = ScKeynodes["notification_storage"]
            notification_class = ScKeynodes["news_notification"]
            nrel_unread = ScKeynodes["nrel_unread"]
            nrel_created_at = ScKeynodes["nrel_created_at"]
            nrel_news_link = ScKeynodes["nrel_news_link"]
            
            added_count = 0
            current_time = datetime.now().isoformat()
            
            for news in news_list:
                # Создаём уведомление
                const = ScConstruction()
                
                # Узел уведомления
                const.create_node(sc_types.NODE_CONST, f"notification_{news.get('id', '')}")
                notification_node_ref = "notification_node"
                
                # Связь с классом уведомлений
                const.create_edge(
                    sc_types.EDGE_ACCESS_CONST_POS_PERM,
                    notification_class,
                    notification_node_ref
                )
                
                # Связь с хранилищем
                const.create_edge(
                    sc_types.EDGE_ACCESS_CONST_POS_PERM,
                    storage,
                    notification_node_ref
                )
                
                # Атрибут "не прочитано" (флаг)
                unread_node_ref = "unread_flag"
                const.create_node(sc_types.NODE_CONST, f"unread_{news.get('id', '')}")
                const.create_edge(
                    sc_types.EDGE_ACCESS_CONST_POS_PERM,
                    nrel_unread,
                    unread_node_ref
                )
                const.create_edge(
                    sc_types.EDGE_ACCESS_CONST_POS_PERM,
                    unread_node_ref,
                    notification_node_ref
                )
                
                # Атрибут "время создания"
                time_link_ref = "time_link"
                const.create_link(sc_types.LINK_CONST, current_time)
                const.create_edge(
                    sc_types.EDGE_D_COMMON_CONST,
                    notification_node_ref,
                    time_link_ref
                )
                const.create_edge(
                    sc_types.EDGE_ACCESS_CONST_POS_PERM,
                    nrel_created_at,
                    sc_types.EDGE_ACCESS_CONST_POS_PERM
                )
                
                # Атрибут "ссылка на новость"
                news_link_ref = "news_url"
                const.create_link(sc_types.LINK_CONST, news.get('link', ''))
                const.create_edge(
                    sc_types.EDGE_D_COMMON_CONST,
                    notification_node_ref,
                    news_link_ref
                )
                const.create_edge(
                    sc_types.EDGE_ACCESS_CONST_POS_PERM,
                    nrel_news_link,
                    sc_types.EDGE_ACCESS_CONST_POS_PERM
                )
                
                # Заголовок новости (как основная идентификация)
                title_link_ref = "title_link"
                const.create_link(sc_types.LINK_CONST, news.get('title', ''))
                const.create_edge(
                    sc_types.EDGE_D_COMMON_CONST,
                    notification_node_ref,
                    title_link_ref
                )
                
                try:
                    addrs = client.create_elements(const)
                    added_count += 1
                    logger.debug(f"Создано уведомление для новости: {news.get('title', '')[:50]}")
                except Exception as e:
                    logger.error(f"Ошибка создания уведомления: {e}")
            
            logger.info(f"Добавлено {added_count} уведомлений")
            return added_count
            
        except Exception as e:
            logger.error(f"Ошибка при добавлении уведомлений: {e}")
            return 0
    
    def get_unread_count(self) -> int:
        """
        Возвращает количество непрочитанных уведомлений.
        """
        try:
            nrel_unread = ScKeynodes["nrel_unread"]
            
            template = ScTemplate()
            template.triple(
                nrel_unread,
                sc_types.EDGE_ACCESS_VAR_POS_PERM >> "_edge",
                sc_types.NODE_VAR >> "_unread_node"
            )
            
            results = client.template_search(template)
            return len(results)
        except Exception as e:
            logger.error(f"Ошибка подсчёта уведомлений: {e}")
            return 0
    
    def mark_as_read(self, notification_addr: ScAddr = None) -> bool:
        """
        Отмечает уведомление(я) как прочитанное.
        Если notification_addr не указан — отмечает все.
        """
        try:
            nrel_unread = ScKeynodes["nrel_unread"]
            
            # Находим все флаги "не прочитано"
            template = ScTemplate()
            template.triple(
                nrel_unread,
                sc_types.EDGE_ACCESS_VAR_POS_PERM >> "_edge",
                sc_types.NODE_VAR >> "_unread_node"
            )
            
            results = client.template_search(template)
            
            for result in results:
                unread_node = result.get("_unread_node")
                if unread_node:
                    client.delete_elements(unread_node)
            
            logger.info(f"Отмечено как прочитано: {len(results)} уведомлений")
            return True
        except Exception as e:
            logger.error(f"Ошибка отметки прочитанных: {e}")
            return False
    
    def get_all_notifications(self, limit: int = 50) -> List[Dict[str, Any]]:
        """
        Получает все уведомления из SC-памяти.
        """
        notifications = []
        
        try:
            notification_class = ScKeynodes["news_notification"]
            nrel_unread = ScKeynodes["nrel_unread"]
            nrel_created_at = ScKeynodes["nrel_created_at"]
            nrel_news_link = ScKeynodes["nrel_news_link"]
            
            # Находим все уведомления
            template = ScTemplate()
            template.triple(
                notification_class,
                sc_types.EDGE_ACCESS_VAR_POS_PERM,
                sc_types.NODE_VAR >> "_notification"
            )
            
            results = client.template_search(template)
            
            for result in results[:limit]:
                notification_node = result.get("_notification")
                if not notification_node:
                    continue
                
                # Получаем заголовок
                title = self._get_node_content(notification_node)
                
                # Проверяем, не прочитано ли
                is_unread = self._check_unread(notification_node, nrel_unread)
                
                # Получаем ссылку
                link = self._get_link_content(notification_node, nrel_news_link)
                
                notifications.append({
                    'addr': notification_node.value,
                    'title': title,
                    'link': link,
                    'unread': is_unread
                })
            
            return notifications
            
        except Exception as e:
            logger.error(f"Ошибка получения уведомлений: {e}")
            return []
    
    def _get_node_content(self, node: ScAddr) -> str:
        """Получает содержимое узла (заголовок)"""
        try:
            template = ScTemplate()
            template.triple(
                node,
                sc_types.EDGE_D_COMMON_VAR,
                sc_types.LINK_VAR >> "_link"
            )
            results = client.template_search(template)
            if results:
                link_addr = results[0].get("_link")
                content = client.get_link_content(link_addr)
                if content:
                    return content[0].data
        except:
            pass
        return "Без заголовка"
    
    def _check_unread(self, node: ScAddr, nrel_unread: ScAddr) -> bool:
        """Проверяет, есть ли флаг непрочитанного"""
        try:
            template = ScTemplate()
            template.triple(
                nrel_unread,
                sc_types.EDGE_ACCESS_VAR_POS_PERM,
                sc_types.NODE_VAR >> "_unread"
            )
            template.triple(
                "_unread",
                sc_types.EDGE_ACCESS_VAR_POS_PERM,
                node
            )
            results = client.template_search(template)
            return len(results) > 0
        except:
            return False
    
    def _get_link_content(self, node: ScAddr, relation: ScAddr) -> str:
        """Получает содержимое связи по отношению"""
        try:
            template = ScTemplate()
            template.quintuple(
                node,
                sc_types.EDGE_D_COMMON_VAR,
                sc_types.LINK_VAR >> "_link",
                sc_types.EDGE_ACCESS_VAR_POS_PERM,
                relation
            )
            results = client.template_search(template)
            if results:
                link_addr = results[0].get("_link")
                content = client.get_link_content(link_addr)
                if content:
                    return content[0].data
        except:
            pass
        return ""
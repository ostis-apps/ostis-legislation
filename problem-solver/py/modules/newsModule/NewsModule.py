from sc_kpm import ScModule
from .RssParserAgent import RssParserAgent
from .RssFilterAgent import RssFilterAgent
from .SchedulerAgent import SchedulerAgent
from .NotificationAgent import NotificationAgent


class NewsModule(ScModule):
    def __init__(self):
        self.parser = RssParserAgent()
        self.filter = RssFilterAgent()
        # SchedulerAgent — обычный класс, не ScAgent; не передаём в super()
        self.scheduler = SchedulerAgent(self.parser, self.filter)
        self.notificator = NotificationAgent()
        super().__init__(self.parser, self.filter, self.notificator)

    def shutdown(self):
        self.scheduler.shutdown()

class RelationHandler:
    def __init__(self, agent_module):
        self.agent_module = agent_module

    def add_pos_relation(self, current_index: int):
        from model.sc_test_telegram_agent import add_pos_relation
        add_pos_relation(current_index)

    def add_neg_relation(self, current_index: int):
        from model.sc_test_telegram_agent import add_neg_relation
        add_neg_relation(current_index)

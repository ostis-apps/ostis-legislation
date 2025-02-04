from ..agent_factory import load_agents


class RecommendationsAgentProxy:
    def __init__(self, agents):
        self.agents = agents

    def execute(self):
        self.agents = load_agents()
        recommendations_agent = self.agents.get("recommendations_agent")

        if recommendations_agent is None:
            raise ValueError("Recommendations agent not found.")
        struct = self.get_question_agent_struct()

        recommendations_agent.execute(struct)

    def get_question_agent_struct(self):
        from model.sc_test_telegram_agent import get_question_agent_struct
        return get_question_agent_struct()

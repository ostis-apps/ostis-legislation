#pragma once

#include <sc-memory/sc_agent.hpp>

class ScUserEventsAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;

    ScResult DoProgram(ScAction & action) override;

    ScAddr get_result_tuple(ScAddr const & user_node, ScStructure & result);
    void remove_old_results(ScAddr const & result_tuple);
    void create_event_data_template(ScAddr const & event_node, ScTemplate & event_data_template);
    void create_new_search_results(ScAddr const & user_node, ScStructure & result);
};
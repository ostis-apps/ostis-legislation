#include "user_events_agent.hpp"
#include <iostream>
#include <sc-memory/sc_memory.hpp>

#include "keynodes/user_events_keynodes.hpp"

using namespace utils;

ScAddr ScUserEventsAgent::GetActionClass() const
{
    return UserEventsKeynodes::action_user_events;
}

ScResult ScUserEventsAgent::DoProgram(ScAction & action)
{
    auto const & [user_node] = action.GetArguments<1>(); 
    ScStructure result = m_context.GenerateStructure();

    if (!m_context.IsElement(user_node)) 
    {
        m_logger.Error("Invalid ScAddr provided.");
        return action.FinishUnsuccessfully();
    }

    if (!m_context.CheckConnector(UserEventsKeynodes::concept_user, user_node, ScType::ConstPermPosArc))
    {
        m_logger.Error("Provided address is not recognized as a user.");
        return action.FinishUnsuccessfully();
    }

    m_logger.Info("Searching for events linked to the user...");
    
    try {
        create_new_search_results(user_node, result);
    }
    catch (ScException const & exception)
    {
        m_logger.Error(exception.Message());
        return action.FinishWithError();
    }

    action.SetResult(result);
    return action.FinishSuccessfully();
}

ScAddr ScUserEventsAgent::get_result_tuple(ScAddr const & user_node, ScStructure & result) {
    ScTemplate tuple_template;
    tuple_template.Quintuple(
        user_node,
        ScType::VarCommonArc,
        ScType::VarNodeTuple >> "_tuple",
        ScType::VarPermPosArc,
        UserEventsKeynodes::nrel_displayed_events
    );
    ScTemplateSearchResult search_result;
    m_context.SearchByTemplate(tuple_template, search_result);

    if (search_result.IsEmpty()) {
        ScAddr const & result_tuple = m_context.GenerateNode(ScType::ConstNodeTuple);
        ScAddr const & result_relation =
            m_context.GenerateConnector(ScType::ConstCommonArc, user_node, result_tuple);
        ScAddr const & relation_membership_arc =
            m_context.GenerateConnector(ScType::ConstPermPosArc, UserEventsKeynodes::nrel_displayed_events, result_relation);

        result << user_node << result_tuple << result_relation << relation_membership_arc;
        return result_tuple;
    }

    result << search_result[0];
    ScAddr const & result_tuple = search_result[0]["_tuple"];
    remove_old_results(result_tuple);
    return result_tuple;
}

void ScUserEventsAgent::remove_old_results(ScAddr const & result_tuple){
    ScIterator3Ptr it = m_context.CreateIterator3(result_tuple, ScType::ConstPermPosArc, ScType::ConstNode);

    while (it->Next())
    {
        ScAddr const & connector = it->Get(1);
        m_context.EraseElement(connector);
    }
}

void ScUserEventsAgent::create_event_data_template(ScAddr const & event_node, ScTemplate & event_data_template){
    event_data_template.Quintuple(
        event_node,
        ScType::VarCommonArc,
        ScType::VarNodeLink,
        ScType::VarPermPosArc,
        UserEventsKeynodes::NREL_EVENT_NAME
    );
    event_data_template.Quintuple(
        event_node,
        ScType::VarCommonArc,
        ScType::VarNodeLink,
        ScType::VarPermPosArc,
        UserEventsKeynodes::NREL_EVENT_DESCRIPTION
    );
    event_data_template.Quintuple(
        event_node,
        ScType::VarCommonArc,
        ScType::VarNode >> "_date",
        ScType::VarPermPosArc,
        UserEventsKeynodes::NREL_EVENT_DATE
    );
    event_data_template.Quintuple(
        ScType::VarNode >> "_date",
        ScType::VarPermPosArc,
        ScType::VarNode,
        ScType::VarPermPosArc,
        UserEventsKeynodes::RREL_EVENT_DAY
    );
    event_data_template.Quintuple(
        ScType::VarNode >> "_date",
        ScType::VarPermPosArc,
        ScType::VarNode,
        ScType::VarPermPosArc,
        UserEventsKeynodes::RREL_EVENT_MONTH
    );
    event_data_template.Quintuple(
        ScType::VarNode >> "_date",
        ScType::VarPermPosArc,
        ScType::VarNode,
        ScType::VarPermPosArc,
        UserEventsKeynodes::RREL_EVENT_YEAR
    );
}

void ScUserEventsAgent::create_new_search_results(ScAddr const & user_node, ScStructure & result) {
    ScAddr const & result_tuple = get_result_tuple(user_node, result);

    ScTemplate event_template;
    event_template.Quintuple(
        user_node,
        ScType::VarCommonArc,
        ScType::VarNode >> "_event",
        ScType::VarPermPosArc,
        UserEventsKeynodes::nrel_user_event
    );
    ScTemplateSearchResult search_result;
    m_context.SearchByTemplate(event_template, search_result);

    if (search_result.IsEmpty()) 
    {
        m_logger.Info("No events found for the user.");
        return;
    }

    m_logger.Info("Total events found: " + std::to_string(search_result.Size()));
    for (size_t i = 0; i < search_result.Size(); i++)
    {
        ScAddr const & event_node = search_result[i]["_event"];

        ScTemplate event_data_template;
        create_event_data_template(event_node, event_data_template);
        ScTemplateSearchResult event_data;
        m_context.SearchByTemplate(event_data_template, event_data);
        if (event_data.IsEmpty()) {
            m_logger.Info("No data found for event, skipping");
            continue;
        }
        
        ScAddr const & connector = m_context.GenerateConnector(ScType::ConstPermPosArc, result_tuple, event_node);

        result << connector << event_data[0];
    }
}

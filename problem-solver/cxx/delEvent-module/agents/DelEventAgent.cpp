#include "DelEventAgent.hpp"
#include "keynodes/DelEventKeynodes.hpp"

ScAddr DelEventAgent::GetActionClass() const
{
  return DelEventKeynodes::action_del_event;
}

ScResult DelEventAgent::DoProgram(ScAction & action)
{
  auto const & [userAddr, eventNameAddr] = action.GetArguments<2>();

  // Check if the user is registered
  if (!m_context.HelperCheckEdge(DelEventKeynodes::REGISTERED_USER, userAddr, ScType::ConstPermPosArc))
  {
    SC_LOG_ERROR("Provided address is not recognized as a user.");
    return action.FinishUnsuccessfully();
  }

  // Get the event name from the link
  std::string eventName;
  if (!m_context.GetLinkContent(eventNameAddr, eventName))
  {
    SC_LOG_ERROR("Failed to get event name from link.");
    return action.FinishUnsuccessfully();
  }

  // Create a template to search for the event using .Triple
  ScTemplate searchTemplate;

  // Describe the connection: userAddr -> _user_event_edge -> _event
  searchTemplate.Triple(
      userAddr,
      ScType::VarCommonArc >> "_user_event_edge",
      ScType::VarNode >> "_event"
  );

  // Describe the relation for the user-event connection: _user_event_edge -> _relation_edge -> nrel_user_event
  searchTemplate.Triple(
      "_user_event_edge",
      ScType::VarPermPosArc >> "_relation_edge",
      DelEventKeynodes::NREL_USER_EVENT
  );

  // Describe the connection: _event -> _event_name_edge -> eventNameAddr
  searchTemplate.Triple(
      "_event",
      ScType::VarCommonArc >> "_event_name_edge",
      eventNameAddr
  );

  // Describe the relation for the event-name connection: _event_name_edge -> _name_relation_edge -> nrel_event_name
  searchTemplate.Triple(
      "_event_name_edge",
      ScType::VarPermPosArc >> "_name_relation_edge",
      DelEventKeynodes::NREL_EVENT_NAME
  );

  ScTemplateSearchResult searchResult;
  m_context.SearchByTemplate(searchTemplate, searchResult);

  if (searchResult.IsEmpty())
  {
    SC_LOG_ERROR("Event with name '" + eventName + "' not found for the user.");
    return action.FinishUnsuccessfully();
  }

  // Assuming one event per user per name; process the first result
  ScTemplateResultItem resultItem;
  searchResult.Get(0, resultItem);

  ScAddr eventAddr;
  resultItem.Get("_event", eventAddr);

  // Remove all elements connected to the event
  ScIterator3Ptr it = m_context.Iterator3(eventAddr, ScType::EdgeAccess, ScType::Unknown);
  while (it->Next())
  {
    ScAddr target = it->Get(2);
    m_context.EraseElement(target);
  }

  // Remove the event node itself
  m_context.EraseElement(eventAddr);

  // Remove the edge connecting the user to the event
  ScAddr userEventEdge;
  resultItem.Get("_user_event_edge", userEventEdge);
  m_context.EraseElement(userEventEdge);

  SC_LOG_INFO("Event successfully removed.");

  return action.FinishSuccessfully();
}

#include "AddEventAgent.hpp"
#include "keynodes/AddEventKeynodes.hpp"
#include <openssl/sha.h>

std::string getEventDay(ScAgentContext& context, const ScAddr& eventdateTuple) {
  ScTemplate eventDayTemplate;

  eventDayTemplate.Quintuple(
      eventdateTuple,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_event_day",
      ScType::VarPermPosArc,
      AddEventKeynodes::RREL_EVENT_DAY
  );

  ScTemplateSearchResult eventDayTemplateSearchResult;
  context.SearchByTemplate(eventDayTemplate, eventDayTemplateSearchResult);

  if (!eventDayTemplateSearchResult.IsEmpty()) {
    ScTemplateResultItem eventDayTemplateResultItem;
    eventDayTemplateSearchResult.Get(0, eventDayTemplateResultItem);

    ScAddr eventDayAddr;

    eventDayTemplateResultItem.Get("_event_day", eventDayAddr);

    return context.GetElementSystemIdentifier(eventDayAddr);
  }

  return "";
}

std::string getEventMonth(ScAgentContext& context, const ScAddr& eventdateTuple) {
  ScTemplate eventMonthTemplate;

  eventMonthTemplate.Quintuple(
      eventdateTuple,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_event_month",
      ScType::VarPermPosArc,
      AddEventKeynodes::RREL_EVENT_MONTH
  );

  ScTemplateSearchResult eventMonthTemplateSearchResult;
  context.SearchByTemplate(eventMonthTemplate, eventMonthTemplateSearchResult);

  if (!eventMonthTemplateSearchResult.IsEmpty()) {
    ScTemplateResultItem eventMonthTemplateResultItem;
    eventMonthTemplateSearchResult.Get(0, eventMonthTemplateResultItem);

    ScAddr eventMonthAddr;

    eventMonthTemplateResultItem.Get("_event_month", eventMonthAddr);

    return context.GetElementSystemIdentifier(eventMonthAddr);
  }

  return "";
}

std::string getEventYear(ScAgentContext& context, const ScAddr& eventdateTuple) {
  ScTemplate eventYearTemplate;

  eventYearTemplate.Quintuple(
      eventdateTuple,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_event_year",
      ScType::VarPermPosArc,
      AddEventKeynodes::RREL_EVENT_YEAR
  );

  ScTemplateSearchResult eventYearTemplateSearchResult;
  context.SearchByTemplate(eventYearTemplate, eventYearTemplateSearchResult);

  if (!eventYearTemplateSearchResult.IsEmpty()) {
    ScTemplateResultItem eventYearTemplateResultItem;
    eventYearTemplateSearchResult.Get(0, eventYearTemplateResultItem);

    ScAddr eventYearAddr;

    eventYearTemplateResultItem.Get("_event_year", eventYearAddr);

    return context.GetElementSystemIdentifier(eventYearAddr);
  }

  return "";
}

ScAddr AddEventAgent::GetActionClass() const
{
  return AddEventKeynodes::action_add_event;
}

ScResult AddEventAgent::DoProgram(ScAction & action)
{

  auto const & [nameAddr, eventdateAddr, descriptionAddr] = action.GetArguments<3>();

  // Данные события
  std::string name;
  std::string description;
  std::string eventDay;
  std::string eventMonth;
  std::string eventYear;

  // Получение данных в переменные
  m_context.GetLinkContent(nameAddr, name);
  m_context.GetLinkContent(descriptionAddr, description);
  eventDay = getEventDay(m_context, eventdateAddr);
  eventMonth = getEventMonth(m_context, eventdateAddr);
  eventYear = getEventYear(m_context, eventdateAddr);

  SC_LOG_INFO("Название: " + name);
  SC_LOG_INFO("Описание: " + description);
  SC_LOG_INFO("Дата события: " + eventDay + "." + eventMonth + "." + eventYear);


  // Генерация узлов для хранения события
  ScAddr eventAddr = m_context.GenerateNode(ScType::ConstNode);
  ScAddr eventNameAddr = m_context.GenerateLink();
  ScAddr eventDescriptionAddr = m_context.GenerateLink();
  ScAddr eventDateAddr = m_context.GenerateNode(ScType::ConstNodeTuple);

  // Заполнение узлов для хранения события
  m_context.SetLinkContent(eventNameAddr, name);
  m_context.SetLinkContent(eventDescriptionAddr, description);

  // Ребра от узла события к его параметрам
  ScAddr eventAddEventToEventNodeConnectorAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, AddEventKeynodes::ADD_EVENT, eventAddr);
  ScAddr eventNodeToEventNameAddr = m_context.GenerateConnector(ScType::ConstCommonArc, eventAddr, eventNameAddr);
  ScAddr eventNodeToEventDateAddr = m_context.GenerateConnector(ScType::ConstCommonArc, eventAddr, eventDateAddr);
  ScAddr eventNodeToEventDescriptionAddr = m_context.GenerateConnector(ScType::ConstCommonArc, eventAddr, eventDescriptionAddr);

  // Ребра от узлов отношений к ребрам
  ScAddr eventNameNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, AddEventKeynodes::NREL_EVENT_NAME, eventNodeToEventNameAddr);
  ScAddr eventDateNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, AddEventKeynodes::NREL_EVENT_DATE, eventNodeToEventDateAddr);
  ScAddr eventDescriptionNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, AddEventKeynodes::NREL_EVENT_DESCRIPTION, eventNodeToEventDescriptionAddr);
  
  return action.FinishSuccessfully();
}

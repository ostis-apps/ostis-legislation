#include "AddTopicAgent.hpp"
#include "../keynodes/AddTopicKeynodes.hpp"

using namespace addTopicModule;

ScAddr AddTopicAgent::GetActionClass() const
{
  return AddTopicKeynodes::action_add_topic;
}

ScResult AddTopicAgent::DoProgram(ScAction & action)
{
  auto const & [authorAddr, titleAddr, descriptionAddr] = action.GetArguments<3>();

  std::string title;
  std::string description;

  m_context.GetLinkContent(titleAddr, title);
  m_context.GetLinkContent(descriptionAddr, description);

  SC_LOG_INFO("Создание темы: " + title);

  ScAddr topicAddr = m_context.GenerateNode(ScType::ConstNode);
  m_context.SetElementSystemIdentifier("topic_" + title, topicAddr);

  ScAddr topicTitleLinkAddr = m_context.GenerateLink();
  ScAddr topicDescriptionLinkAddr = m_context.GenerateLink();

  m_context.SetLinkContent(topicTitleLinkAddr, title);
  m_context.SetLinkContent(topicDescriptionLinkAddr, description);

  m_context.GenerateConnector(ScType::ConstPermPosArc, AddTopicKeynodes::TOPIC_CLASS, topicAddr);

  ScAddr topicToTitleArc = m_context.GenerateConnector(ScType::ConstCommonArc, topicAddr, topicTitleLinkAddr);
  m_context.GenerateConnector(ScType::ConstPermPosArc, AddTopicKeynodes::NREL_TOPIC_TITLE, topicToTitleArc);

  ScAddr topicToDescriptionArc = m_context.GenerateConnector(ScType::ConstCommonArc, topicAddr, topicDescriptionLinkAddr);
  m_context.GenerateConnector(ScType::ConstPermPosArc, AddTopicKeynodes::NREL_TOPIC_DESCRIPTION, topicToDescriptionArc);

  ScAddr topicToAuthorArc = m_context.GenerateConnector(ScType::ConstCommonArc, topicAddr, authorAddr);
  m_context.GenerateConnector(ScType::ConstPermPosArc, AddTopicKeynodes::NREL_TOPIC_AUTHOR, topicToAuthorArc);

  SC_LOG_INFO("Тема успешно создана: " + title);
  return action.FinishSuccessfully();
}

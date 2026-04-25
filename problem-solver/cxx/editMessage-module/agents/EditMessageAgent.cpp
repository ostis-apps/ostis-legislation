#include "EditMessageAgent.hpp"
#include "../keynodes/keynodes.hpp"

using namespace editMessageModule;

ScAddr EditMessageAgent::GetActionClass() const
{
  return Keynodes::action_edit_message;
}

ScResult EditMessageAgent::DoProgram(ScAction & action)
{
  auto const & [messageAddr, newContentLink] = action.GetArguments<2>();

  if (!messageAddr.IsValid() || !newContentLink.IsValid())
  {
    SC_LOG_ERROR("EditMessageAgent: invalid arguments");
    return action.FinishWithError();
  }

  std::string newText;
  m_context.GetLinkContent(newContentLink, newText);

  SC_LOG_INFO("EditMessageAgent: editing message");

  // Находим и удаляем старый контент
  ScTemplate searchContent;
  searchContent.Quintuple(
    messageAddr,
    ScType::VarCommonArc >> "contentArc",
    ScType::VarNode >> "contentLink",
    ScType::VarPermPosArc >> "contentRelArc",
    Keynodes::nrel_message_content
  );
  ScTemplateSearchResult contentResult;
  m_context.SearchByTemplate(searchContent, contentResult);

  if (contentResult.Size() > 0)
  {
    m_context.EraseElement(contentResult[0]["contentLink"]);
    m_context.EraseElement(contentResult[0]["contentArc"]);
    m_context.EraseElement(contentResult[0]["contentRelArc"]);
  }

  // Создаём новый контент
  ScAddr newLink = m_context.GenerateLink();
  m_context.SetLinkContent(newLink, newText);
  ScAddr newArc = m_context.GenerateConnector(ScType::ConstCommonArc, messageAddr, newLink);
  m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_message_content, newArc);

  SC_LOG_INFO("EditMessageAgent: message edited successfully");
  return action.FinishSuccessfully();
}
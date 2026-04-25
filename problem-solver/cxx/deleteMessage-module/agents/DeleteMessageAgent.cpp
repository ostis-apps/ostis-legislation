#include "DeleteMessageAgent.hpp"
#include "../keynodes/keynodes.hpp"

using namespace deleteMessageModule;

ScAddr DeleteMessageAgent::GetActionClass() const
{
  return Keynodes::action_delete_message;
}

ScResult DeleteMessageAgent::DoProgram(ScAction & action)
{
  auto const & [messageAddr] = action.GetArguments<1>();

  if (!messageAddr.IsValid())
  {
    SC_LOG_ERROR("DeleteMessageAgent: messageAddr is not valid");
    return action.FinishWithError();
  }

  SC_LOG_INFO("Удаление сообщения");

  // Удаляем дугу concept_message -> messageNode
  ScAddr conceptMessage = m_context.SearchElementBySystemIdentifier("concept_message");
  if (conceptMessage.IsValid())
  {
    ScTemplate searchConceptArc;
    searchConceptArc.Triple(
      conceptMessage,
      ScType::VarPermPosArc >> "conceptArc",
      messageAddr
    );
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchConceptArc, result);
    if (result.Size() > 0)
      m_context.EraseElement(result[0]["conceptArc"]);
  }

  // Удаляем дугу topicNode -> messageNode
  ScTemplate searchTopicArc;
  searchTopicArc.Triple(
    ScType::VarNode >> "topicNode",
    ScType::VarPermPosArc >> "topicArc",
    messageAddr
  );
  ScTemplateSearchResult topicResult;
  m_context.SearchByTemplate(searchTopicArc, topicResult);
  if (topicResult.Size() > 0)
    m_context.EraseElement(topicResult[0]["topicArc"]);

  // Удаляем nrel_message_content
  ScAddr nrelMessageContent = m_context.SearchElementBySystemIdentifier("nrel_message_content");
  if (nrelMessageContent.IsValid())
  {
    ScTemplate searchContent;
    searchContent.Quintuple(
      messageAddr,
      ScType::VarCommonArc >> "contentArc",
      ScType::VarNode >> "contentLink",
      ScType::VarPermPosArc >> "contentRelArc",
      nrelMessageContent
    );
    ScTemplateSearchResult contentResult;
    m_context.SearchByTemplate(searchContent, contentResult);
    if (contentResult.Size() > 0)
    {
      m_context.EraseElement(contentResult[0]["contentLink"]);
      m_context.EraseElement(contentResult[0]["contentArc"]);
      m_context.EraseElement(contentResult[0]["contentRelArc"]);
    }
  }

  // Удаляем nrel_author
  ScAddr nrelMessageAuthor = m_context.SearchElementBySystemIdentifier("nrel_author");
  if (nrelMessageAuthor.IsValid())
  {
    ScTemplate searchAuthor;
    searchAuthor.Quintuple(
      messageAddr,
      ScType::VarCommonArc >> "authorArc",
      ScType::VarNode >> "authorNode",
      ScType::VarPermPosArc >> "authorRelArc",
      nrelMessageAuthor
    );
    ScTemplateSearchResult authorResult;
    m_context.SearchByTemplate(searchAuthor, authorResult);
    if (authorResult.Size() > 0)
    {
      m_context.EraseElement(authorResult[0]["authorArc"]);
      m_context.EraseElement(authorResult[0]["authorRelArc"]);
    }
  }

  // Удаляем сам узел сообщения
  m_context.EraseElement(messageAddr);

  SC_LOG_INFO("Сообщение успешно удалено");
  return action.FinishSuccessfully();
}

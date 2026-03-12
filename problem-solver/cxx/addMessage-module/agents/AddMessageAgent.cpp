#include "AddMessageAgent.hpp"
#include "../keynodes/keynodes.hpp"

using namespace addMessageModule;

ScAddr AddMessageAgent::GetActionClass() const
{
    return Keynodes::action_add_message;
}

ScResult AddMessageAgent::DoProgram(ScAction & action)
{
    auto const & [authorAddr, topicAddr, messageContentLink] = action.GetArguments<3>();
    
    std::string messageText;
    m_context.GetLinkContent(messageContentLink, messageText);
    
    SC_LOG_INFO("Добавление сообщения в топик");
    
    // Создаем узел сообщения
    ScAddr messageNode = m_context.GenerateNode(ScType::ConstNode);
    
    // Добавляем в класс concept_message
    ScAddr conceptMessage = m_context.SearchElementBySystemIdentifier("concept_message");
    m_context.GenerateConnector(ScType::ConstPermPosArc, conceptMessage, messageNode);
    
    // Связываем топик и сообщение
    m_context.GenerateConnector(ScType::ConstPermPosArc, topicAddr, messageNode);
    
    // Создаем link для содержимого сообщения
    ScAddr messageLink = m_context.GenerateLink();
    m_context.SetLinkContent(messageLink, messageText);
    
    // Связываем сообщение с содержимым через nrel_message_content
    ScAddr nrelMessageContent = m_context.SearchElementBySystemIdentifier("nrel_message_content");
    ScAddr contentArc = m_context.GenerateConnector(ScType::ConstCommonArc, messageNode, messageLink);
    m_context.GenerateConnector(ScType::ConstPermPosArc, nrelMessageContent, contentArc);
    
    // Связываем сообщение с автором через nrel_message_author
    ScAddr nrelMessageAuthor = m_context.SearchElementBySystemIdentifier("nrel_message_author");
    ScAddr authorArc = m_context.GenerateConnector(ScType::ConstCommonArc, messageNode, authorAddr);
    m_context.GenerateConnector(ScType::ConstPermPosArc, nrelMessageAuthor, authorArc);
    
    SC_LOG_INFO("Сообщение успешно добавлено");
    
    return action.FinishSuccessfully();
}

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

  // Узел сообщения
  ScAddr messageNode = m_context.GenerateNode(ScType::ConstNode);

  // Класс concept_message
  m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_message, messageNode);

  // Связь топик -> сообщение
  m_context.GenerateConnector(ScType::ConstPermPosArc, topicAddr, messageNode);

  // Текст сообщения
  ScAddr messageLink = m_context.GenerateLink();
  m_context.SetLinkContent(messageLink, messageText);
  ScAddr contentArc = m_context.GenerateConnector(ScType::ConstCommonArc, messageNode, messageLink);
  m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_message_content, contentArc);

  // Автор
  ScAddr authorArc = m_context.GenerateConnector(ScType::ConstCommonArc, messageNode, authorAddr);
  m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_message_author, authorArc);

  // Вложение (rrel_4 = base64, rrel_5 = имя файла, rrel_6 = mime-тип) — опционально
  ScIterator5Ptr attachIt = m_context.CreateIterator5(
    action,
    ScType::ConstPermPosArc,
    ScType::ConstNodeLink,
    ScType::ConstPermPosArc,
    Keynodes::rrel_4
  );

  if (attachIt->Next())
  {
    ScAddr base64Link = attachIt->Get(2);

    // Узел вложения
    ScAddr attachNode = m_context.GenerateNode(ScType::ConstNode);
    ScAddr attachArc = m_context.GenerateConnector(ScType::ConstCommonArc, messageNode, attachNode);
    m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_message_attachment, attachArc);

    // base64 данные
    ScAddr dataArc = m_context.GenerateConnector(ScType::ConstCommonArc, attachNode, base64Link);
    m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_attachment_data, dataArc);

    // Имя файла (rrel_5)
    ScIterator5Ptr nameIt = m_context.CreateIterator5(
      action, ScType::ConstPermPosArc, ScType::ConstNodeLink,
      ScType::ConstPermPosArc, Keynodes::rrel_5
    );
    if (nameIt->Next())
    {
      ScAddr nameArc = m_context.GenerateConnector(ScType::ConstCommonArc, attachNode, nameIt->Get(2));
      m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_attachment_name, nameArc);
    }

    // MIME-тип (rrel_6)
    ScIterator5Ptr mimeIt = m_context.CreateIterator5(
      action, ScType::ConstPermPosArc, ScType::ConstNodeLink,
      ScType::ConstPermPosArc, Keynodes::rrel_6
    );
    if (mimeIt->Next())
    {
      ScAddr mimeArc = m_context.GenerateConnector(ScType::ConstCommonArc, attachNode, mimeIt->Get(2));
      m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::nrel_attachment_mime, mimeArc);
    }

    SC_LOG_INFO("Вложение сохранено");
  }

  SC_LOG_INFO("Сообщение успешно добавлено");
  return action.FinishSuccessfully();
}
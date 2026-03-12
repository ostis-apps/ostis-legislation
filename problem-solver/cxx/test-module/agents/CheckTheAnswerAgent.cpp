#include "CheckTheAnswerAgent.hpp"

#include "keynodes/CheckTheAnswerKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace utils;

ScAddr CheckTheAnswerAgent::GetActionClass() const
{
  return Keynodes::action_check_answer;
}

ScResult CheckTheAnswerAgent::DoProgram(ScAction & action)
{
  CheckTheAnswerKeynodes::Initialize(&m_context);

  auto const & [question, user] = action.GetArguments<2>();

  SC_LOG_DEBUG("CheckTheAnswerAgent: Checking answer for question " << question.Hash() << " and user " << user.Hash());

  // Ищем ответ пользователя
  ScTemplate user_answer_template;
  user_answer_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_selected_answers",
      ScType::VarPermPosArc,
      CheckTheAnswerKeynodes::nrel_selected_answers);
  user_answer_template.Triple(
      "_selected_answers",
      ScType::VarPermPosArc,
      ScType::VarNode >> "_user_answer");

  ScTemplateSearchResult user_answer_search_result;
  if (!m_context.SearchByTemplate(user_answer_template, user_answer_search_result)
      || user_answer_search_result.Size() == 0)
  {
    SC_LOG_ERROR("CheckTheAnswerAgent: User answer not found");
    return action.FinishUnsuccessfully();
  }

  ScTemplateResultItem user_answer_item = user_answer_search_result[user_answer_search_result.Size() - 1];
  ScAddr user_answer;
  user_answer_item.Get("_user_answer", user_answer);  // ИСПРАВЛЕНО: два параметра

  SC_LOG_DEBUG("CheckTheAnswerAgent: User answer = " << user_answer.Hash());

  // Ищем правильный ответ
  ScTemplate correct_answer_template;
  correct_answer_template.Triple(
      CheckTheAnswerKeynodes::concept_correct_answer,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_correct_answer");
  correct_answer_template.Quintuple(
      question,
      ScType::VarCommonArc,
      "_correct_answer",
      ScType::VarPermPosArc,
      CheckTheAnswerKeynodes::nrel_answer);

  ScTemplateSearchResult correct_answer_search_result;
  if (!m_context.SearchByTemplate(correct_answer_template, correct_answer_search_result)
      || correct_answer_search_result.Size() == 0)
  {
    SC_LOG_ERROR("CheckTheAnswerAgent: Correct answer not found");
    return action.FinishUnsuccessfully();
  }

  ScTemplateResultItem correct_answer_item = correct_answer_search_result[0];
  ScAddr correct_answer;
  correct_answer_item.Get("_correct_answer", correct_answer);  // ИСПРАВЛЕНО: два параметра

  SC_LOG_DEBUG("CheckTheAnswerAgent: Correct answer = " << correct_answer.Hash());

  // Сравниваем ответы
  bool is_correct = (user_answer == correct_answer);

  SC_LOG_DEBUG("CheckTheAnswerAgent: Answer is " << (is_correct ? "correct" : "incorrect"));

  // Создаем узел пары (question, user)
  ScAddr answer_check_node = m_context.GenerateNode(ScType::ConstNode);

  // Связываем с вопросом
  ScAddr question_arc = m_context.GenerateConnector(ScType::ConstCommonArc, answer_check_node, question);
  m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::rrel_1, question_arc);

  // Связываем с пользователем
  ScAddr user_arc = m_context.GenerateConnector(ScType::ConstCommonArc, answer_check_node, user);
  m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::rrel_2, user_arc);

  if (is_correct)
  {
    // Отмечаем как правильный ответ
    m_context.GenerateConnector(
        ScType::ConstPermPosArc,
        CheckTheAnswerKeynodes::concept_correct_user_answer,
        answer_check_node);
  }
  else
  {
    // Отмечаем как неправильный ответ
    m_context.GenerateConnector(
        ScType::ConstPermPosArc,
        CheckTheAnswerKeynodes::concept_incorrect_user_answer,
        answer_check_node);
  }

  // Возвращаем результат проверки через nrel_result
  ScAddr result_structure = m_context.GenerateNode(ScType::ConstNodeStructure);
  
  // Создаем link с результатом (1 = правильно, 0 = неправильно)
  ScAddr result_link = m_context.GenerateLink(ScType::ConstNodeLink);
  m_context.SetLinkContent(result_link, ScStreamConverter::StreamFromString(is_correct ? "1" : "0"));
  
  m_context.GenerateConnector(ScType::ConstPermPosArc, result_structure, result_link);

  ScAddr nrel_result = m_context.SearchElementBySystemIdentifier("nrel_result");
  if (nrel_result.IsValid())
  {
    ScAddr action_node = action;
    ScAddr result_arc = m_context.GenerateConnector(ScType::ConstCommonArc, action_node, result_structure);
    m_context.GenerateConnector(ScType::ConstPermPosArc, nrel_result, result_arc);
  }

  return action.FinishSuccessfully();
}

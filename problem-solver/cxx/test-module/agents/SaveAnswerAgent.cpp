#include "SaveAnswerAgent.hpp"

#include "keynodes/SaveAnswerKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace utils;

ScAddr SaveAnswerAgent::GetActionClass() const
{
  return Keynodes::action_save_answer;
}

ScResult SaveAnswerAgent::DoProgram(ScAction & action)
{
  SaveAnswerKeynodes::Initialize(&m_context);

  auto const & [answer, user] = action.GetArguments<2>();

  SC_LOG_DEBUG("SaveAnswerAgent: Saving answer " << answer.Hash() << " for user " << user.Hash());

  // Ищем или создаем множество выбранных ответов пользователя
  ScTemplate selected_answers_template;
  selected_answers_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_selected_answers",
      ScType::VarPermPosArc,
      SaveAnswerKeynodes::nrel_selected_answers);

  ScTemplateSearchResult selected_answers_result;
  ScAddr selected_answers_set;
  
  if (m_context.SearchByTemplate(selected_answers_template, selected_answers_result)
      && selected_answers_result.Size() > 0)
  {
    // Множество уже существует - ИСПРАВЛЕНО: два параметра
    selected_answers_result[0].Get("_selected_answers", selected_answers_set);
    SC_LOG_DEBUG("SaveAnswerAgent: Found existing selected_answers set");
  }
  else
  {
    // Создаем новое множество
    selected_answers_set = m_context.GenerateNode(ScType::ConstNode);
    
    ScAddr arc = m_context.GenerateConnector(ScType::ConstCommonArc, user, selected_answers_set);
    m_context.GenerateConnector(ScType::ConstPermPosArc, SaveAnswerKeynodes::nrel_selected_answers, arc);
    
    SC_LOG_DEBUG("SaveAnswerAgent: Created new selected_answers set");
  }

  // Добавляем ответ в множество
  m_context.GenerateConnector(ScType::ConstPermPosArc, selected_answers_set, answer);

  SC_LOG_DEBUG("SaveAnswerAgent: Answer saved successfully");

  return action.FinishSuccessfully();
}

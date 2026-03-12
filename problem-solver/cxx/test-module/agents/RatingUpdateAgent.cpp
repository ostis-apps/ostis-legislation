#include "RatingUpdateAgent.hpp"

#include "keynodes/RatingUpdateKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace utils;

ScAddr RatingUpdateAgent::GetActionClass() const
{
  return Keynodes::action_update_rating;
}

ScResult RatingUpdateAgent::DoProgram(ScAction & action)
{
  RatingUpdateKeynodes::Initialize(&m_context);
  
  auto const & [user] = action.GetArguments<1>();
  
  SC_LOG_DEBUG("RatingUpdateAgent: Starting for user " << user.Hash());
  
  // Ищем все ответы пользователя через nrel_selected_answers
  ScTemplate user_answers_template;
  user_answers_template.Quintuple(
    user,
    ScType::VarCommonArc,
    ScType::VarNode >> "_selected_answers_set",
    ScType::VarPermPosArc,
    RatingUpdateKeynodes::nrel_selected_answers);
  user_answers_template.Triple(
    "_selected_answers_set",
    ScType::VarPermPosArc,
    ScType::VarNode >> "_user_answer");
  
  ScTemplateSearchResult user_answers_result;
  if (!m_context.SearchByTemplate(user_answers_template, user_answers_result))
  {
    SC_LOG_ERROR("RatingUpdateAgent: No answers found for user");
    return action.FinishUnsuccessfully();
  }
  
  int total_questions = user_answers_result.Size();
  int correct_answers = 0;
  
  SC_LOG_DEBUG("RatingUpdateAgent: Found " << total_questions << " answered questions");
  
  // Для каждого ответа пользователя проверяем правильность
  for (size_t i = 0; i < user_answers_result.Size(); ++i)
  {
    ScTemplateResultItem answer_item = user_answers_result[i];
    ScAddr user_answer;
    answer_item.Get("_user_answer", user_answer);
    
    // Проверяем, есть ли этот ответ в concept_correct_answer
    ScTemplate correct_check_template;
    correct_check_template.Triple(
      RatingUpdateKeynodes::concept_correct_answer,
      ScType::VarPermPosArc,
      user_answer);
    
    ScTemplateSearchResult correct_check_result;
    if (m_context.SearchByTemplate(correct_check_template, correct_check_result) 
        && correct_check_result.Size() > 0)
    {
      correct_answers++;
      SC_LOG_DEBUG("RatingUpdateAgent: Answer " << user_answer.Hash() << " is correct");
    }
    else
    {
      SC_LOG_DEBUG("RatingUpdateAgent: Answer " << user_answer.Hash() << " is incorrect");
    }
  }
  
  SC_LOG_DEBUG("RatingUpdateAgent: Correct answers = " << correct_answers << " / " << total_questions);
  
  // РАССЧИТЫВАЕМ ПРОЦЕНТ И ОПРЕДЕЛЯЕМ РАНГ
  std::string rank;
  if (total_questions == 0)
  {
    rank = "первый ранг";
  }
  else
  {
    double percentage = (static_cast<double>(correct_answers) / total_questions) * 100.0;
    
    if (percentage >= 80.0)
      rank = "третий ранг";
    else if (percentage >= 50.0)
      rank = "второй ранг";
    else
      rank = "первый ранг";
  }
  
  SC_LOG_INFO("RatingUpdateAgent: Assigned rank: " << rank);
  
  // Создаем результат с рангом
  ScAddr result_structure = m_context.GenerateNode(ScType::ConstNodeStructure);
  
  // Создаем link с рангом (РУССКИЙ ТЕКСТ!)
  ScAddr rating_link = m_context.GenerateLink(ScType::ConstNodeLink);
  m_context.SetLinkContent(rating_link, ScStreamConverter::StreamFromString(rank));
  m_context.GenerateConnector(ScType::ConstPermPosArc, result_structure, rating_link);
  
  // Связываем результат с действием через nrel_result
  ScAddr nrel_result = m_context.SearchElementBySystemIdentifier("nrel_result");
  if (nrel_result.IsValid())
  {
    ScAddr action_node = action;
    ScAddr result_arc = m_context.GenerateConnector(ScType::ConstCommonArc, action_node, result_structure);
    m_context.GenerateConnector(ScType::ConstPermPosArc, nrel_result, result_arc);
  }
  
  SC_LOG_DEBUG("RatingUpdateAgent: Finished successfully with rank: " << rank);
  return action.FinishSuccessfully();
}

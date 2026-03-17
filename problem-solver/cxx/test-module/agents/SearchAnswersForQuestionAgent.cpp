#include "SearchAnswersForQuestionAgent.hpp"
#include "keynodes/SearchAnswersForQuestionKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace utils;

ScAddr SearchAnswersForQuestionAgent::GetActionClass() const
{
  return Keynodes::action_search_answers_for_question;
}

ScResult SearchAnswersForQuestionAgent::DoProgram(ScAction & action)
{
  // Инициализируем keynodes
  SearchAnswersForQuestionKeynodes::Initialize(&m_context);
  
  auto const & [question] = action.GetArguments<1>();

  ScTemplate answers_template;
  answers_template.Quintuple(
      question,
      ScType::VarCommonArc,
      ScType::VarNode >> "_answer",
      ScType::VarPermPosArc,
      SearchAnswersForQuestionKeynodes::nrel_answer);

  ScTemplateSearchResult answers_search_result;
  if (!m_context.SearchByTemplate(answers_template, answers_search_result) 
      || answers_search_result.Size() == 0)
  {
    return action.FinishUnsuccessfully();
  }

  return action.FinishSuccessfully();
}

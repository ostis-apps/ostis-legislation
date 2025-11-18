#include "SearchAnswersForQuestionAgent.hpp"
#include "keynodes/SearchAnswersForQuestionKeynodes.hpp"

using namespace utils;

ScResult SearchAnswersForQuestionAgent::DoProgram(ScAction & action)
{
  auto const & [question] = action.GetArguments<1>();

  ScTemplate answers_for_question_template;

  answers_for_question_template.Triple(
      question, SearchAnswersForQuestionKeynodes::nrel_answer, ScType::VarNode >> "_answer");

  ScTemplateSearchResult answers_for_question_search_result;

  if (m_context.SearchByTemplate(answers_for_question_template, answers_for_question_search_result)
      && answers_for_question_search_result.Size() > 0)
  {
    ScStructure resultStruct = m_context.GenerateStructure();

    for (size_t i = 0; i < answers_for_question_search_result.Size(); ++i)
    {
      ScTemplateResultItem answers_for_question_item;
      answers_for_question_search_result.Get(i, answers_for_question_item);

      ScAddr answer;
      answers_for_question_item.Get("_answer", answer);

      resultStruct.Append(answer);
    }

    action.SetResult(resultStruct);
    return action.FinishSuccessfully();
  }
  else
  {
    return action.FinishUnsuccessfully();
  }
}

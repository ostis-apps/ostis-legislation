#include "SaveAnswerAgent.hpp"
#include "keynodes/SaveAnswerKeynodes.hpp"

using namespace utils;

ScResult SaveAnswerAgent::DoProgram(ScAction & action)
{
  auto const & [answer, user] = action.GetArguments<2>();

  ScTemplate user_answers_template;

  user_answers_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_selected_answers",
      ScType::VarPermPosArc,
      SaveAnswerKeynodes::nrel_selected_answers);

  ScTemplateSearchResult user_answers_search_result;

  if (m_context.SearchByTemplate(user_answers_template, user_answers_search_result)
      && user_answers_search_result.Size() == 1)
  {
    ScTemplateResultItem user_answers_item;
    user_answers_search_result.Get(0, user_answers_item);
    ScAddr selected_answers;
    user_answers_item.Get("_selected_answers", selected_answers);

    m_context.GenerateConnector(ScType::ConstPermPosArc, selected_answers, answer);

    return action.FinishSuccessfully();
  }
  else
  {
    ScAddr selected_answers = m_context.GenerateNode(ScType::ConstNodeTuple);

    ScAddr selected_answers_connector = m_context.GenerateConnector(ScType::ConstCommonArc, user, selected_answers);
    m_context.GenerateConnector(
        ScType::ConstPermPosArc, SaveAnswerKeynodes::nrel_selected_answers, selected_answers_connector);

    m_context.GenerateConnector(ScType::ConstPermPosArc, selected_answers, answer);
    return action.FinishSuccessfully();
  }
}

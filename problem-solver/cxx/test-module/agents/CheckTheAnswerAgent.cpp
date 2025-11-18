#include "CheckTheAnswerAgent.hpp"
#include "keynodes/CheckTheAnswerKeynodes.hpp"
#include "keynodes/DeleteOldNodesKeynodes.hpp"

using namespace utils;

ScResult CheckTheAnswerAgent::DoProgram(ScAction & action)
{
  auto const & [question, user] = action.GetArguments<2>();

  ScTemplate user_answers_template;

  user_answers_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_asked_questions",
      ScType::VarPermPosArc,
      CheckTheAnswerKeynodes::nrel_asked_questions);

  user_answers_template.Triple("_asked_questions", ScType::VarPermPosArc, question);

  user_answers_template.Quintuple(
      question,
      ScType::VarCommonArc,
      ScType::VarNode >> "_user_answer",
      ScType::VarPermPosArc,
      CheckTheAnswerKeynodes::nrel_answer);

  user_answers_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_selected_answers",
      ScType::VarPermPosArc,
      CheckTheAnswerKeynodes::nrel_selected_answers);

  user_answers_template.Triple("_selected_answers", ScType::VarPermPosArc, "_user_answer");

  ScTemplateSearchResult user_answers_search_result;

  if (m_context.SearchByTemplate(user_answers_template, user_answers_search_result)
      && user_answers_search_result.Size() > 0)
  {
    ScTemplateResultItem user_answers_item;
    user_answers_search_result.Get(0, user_answers_item);
    ScAddr user_answer;
    user_answers_item.Get("_user_answer", user_answer);

    ScTemplate is_in_concept_right_template;

    is_in_concept_right_template.Triple(
        CheckTheAnswerKeynodes ::concept_correct_answer, ScType::VarPermPosArc, user_answer);

    ScTemplateSearchResult is_right_search_result;
    m_context.SearchByTemplate(is_in_concept_right_template, is_right_search_result);

    if (is_right_search_result.Size() > 0)
    {
      return action.FinishSuccessfully();
    }
    else
    {
      for (size_t i = 0; user_answers_search_result.Size(); i++)
      {
        ScTemplateResultItem user_answers_item;
        user_answers_search_result.Get(i, user_answers_item);
        ScAddr user_answer;
        user_answers_item.Get("_user_answer", user_answer);

        ScTemplateSearchResult is_right_search_result;
        m_context.SearchByTemplate(is_in_concept_right_template, is_right_search_result);

        if (is_right_search_result.Size() > 0)
        {
          continue;
        }
        else
        {
          ScAddr deleteOldNodesAction = m_context.GenerateNode(ScType::ConstNodeClass);
          m_context.GenerateConnector(
              ScType::ConstPermPosArc, DeleteOldNodesKeynodes::action_delete_old_nodes, deleteOldNodesAction);

          ScAddr argsSet_2 = m_context.GenerateNode(ScType::ConstNodeTuple);
          m_context.GenerateConnector(ScType::ConstPermPosArc, argsSet_2, user);
          m_context.GenerateConnector(ScType::ConstPermPosArc, deleteOldNodesAction, argsSet_2);

          ScAction deleteAction = m_context.ConvertToAction(deleteOldNodesAction);
          deleteAction.Initiate();

          return action.FinishUnsuccessfully();
        }
      }
    }
  }
  else
  {
    return action.FinishUnsuccessfully();
  }
}

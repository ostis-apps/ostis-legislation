#include "DeleteOldNodesAgent.hpp"
#include "keynodes/DeleteOldNodesKeynodes.hpp"

using namespace utils;

ScResult DeleteOldNodesAgent::DoProgram(ScAction & action)
{
  auto const & [user] = action.GetArguments<1>();

  ScTemplate old_nodes_template;

  old_nodes_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_asked_questions",
      ScType::VarPermPosArc,
      DeleteOldNodesKeynodes::nrel_asked_questions);

  old_nodes_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_selected_answers",
      ScType::VarPermPosArc,
      DeleteOldNodesKeynodes::nrel_selected_answers);

  ScTemplateSearchResult old_nodes_search_result;

  if (m_context.SearchByTemplate(old_nodes_template, old_nodes_search_result) && old_nodes_search_result.Size() > 0)
  {
    ScTemplateResultItem old_nodes_item;
    old_nodes_search_result.Get(0, old_nodes_item);

    ScAddr asked_questions;
    old_nodes_item.Get("_asked_questions", asked_questions);
    m_context.EraseElement(asked_questions);

    ScAddr selected_answers;
    old_nodes_item.Get("_selected_answers", selected_answers);
    m_context.EraseElement(selected_answers);
    return action.FinishSuccessfully();
  }
  else
  {
    return action.FinishUnsuccessfully();
  }
}

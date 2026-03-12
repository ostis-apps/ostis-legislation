#include "DeleteOldNodesAgent.hpp"
#include "keynodes/DeleteOldNodesKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace utils;

ScAddr DeleteOldNodesAgent::GetActionClass() const
{
  return Keynodes::action_delete_old_nodes;
}

ScResult DeleteOldNodesAgent::DoProgram(ScAction & action)
{
  // Инициализируем keynodes
  DeleteOldNodesKeynodes::Initialize(&m_context);
  
  auto const & [user] = action.GetArguments<1>();

  // Удаляем старые вопросы
  ScTemplate asked_questions_template;
  asked_questions_template.Quintuple(
      user,
      ScType::VarCommonArc >> "_arc",
      ScType::VarNode >> "_asked_questions",
      ScType::VarPermPosArc >> "_attr_arc",
      DeleteOldNodesKeynodes::nrel_asked_questions);

  ScTemplateSearchResult asked_questions_search_result;
  if (m_context.SearchByTemplate(asked_questions_template, asked_questions_search_result))
  {
    for (size_t i = 0; i < asked_questions_search_result.Size(); ++i)
    {
      ScAddr arc, attr_arc, asked_questions;
      asked_questions_search_result[i].Get("_arc", arc);
      asked_questions_search_result[i].Get("_attr_arc", attr_arc);
      asked_questions_search_result[i].Get("_asked_questions", asked_questions);

      m_context.EraseElement(arc);
      m_context.EraseElement(attr_arc);
      m_context.EraseElement(asked_questions);
    }
  }

  // Удаляем старые ответы
  ScTemplate selected_answers_template;
  selected_answers_template.Quintuple(
      user,
      ScType::VarCommonArc >> "_arc",
      ScType::VarNode >> "_selected_answers",
      ScType::VarPermPosArc >> "_attr_arc",
      DeleteOldNodesKeynodes::nrel_selected_answers);

  ScTemplateSearchResult selected_answers_search_result;
  if (m_context.SearchByTemplate(selected_answers_template, selected_answers_search_result))
  {
    for (size_t i = 0; i < selected_answers_search_result.Size(); ++i)
    {
      ScAddr arc, attr_arc, selected_answers;
      selected_answers_search_result[i].Get("_arc", arc);
      selected_answers_search_result[i].Get("_attr_arc", attr_arc);
      selected_answers_search_result[i].Get("_selected_answers", selected_answers);

      m_context.EraseElement(arc);
      m_context.EraseElement(attr_arc);
      m_context.EraseElement(selected_answers);
    }
  }

  return action.FinishSuccessfully();
}

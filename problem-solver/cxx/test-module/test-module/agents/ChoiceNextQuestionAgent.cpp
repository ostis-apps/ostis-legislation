#include "ChoiceNextQuestionAgent.hpp"
#include "keynodes/ChoiceNextQuestionKeynodes.hpp"
#include "keynodes/DeleteOldNodesKeynodes.hpp"
#include "keynodes/RatingUpdateKeynodes.hpp"

#include <openssl/sha.h>

using namespace utils;

bool ChoiceNextQuestionAgent::IsQuestionAsked(
    ScTemplateSearchResult const & asked_questions_search_result,
    std::string id_of_question)
{
  for (size_t j = 0; j < asked_questions_search_result.Size(); ++j)
  {
    ScTemplateResultItem asked_questions_item;
    asked_questions_search_result.Get(j, asked_questions_item);

    ScAddr asked_question;
    asked_questions_item.Get("_asked_question", asked_question);

    if (m_context.GetElementSystemIdentifier(asked_question) == id_of_question)
    {
      return true;
    }
  }
  return false;
}

ScAddr ChoiceNextQuestionAgent::FindNextUnaskedQuestion(
    ScTemplateSearchResult const & asked_questions_search_result,
    ScTemplateSearchResult const & all_questions_search_result)
{
  for (size_t i = 0; i < all_questions_search_result.Size(); ++i)
  {
    ScTemplateResultItem all_questions_item;
    all_questions_search_result.Get(i, all_questions_item);

    ScAddr question;
    all_questions_item.Get("_question", question);
    std::string id_of_question = m_context.GetElementSystemIdentifier(question);

    if (!IsQuestionAsked(asked_questions_search_result, id_of_question))
    {
      return question;
    }
  }
  return false;
}

ScResult ChoiceNextQuestionAgent::DoProgram(ScAction & action)
{
  auto const & [test, user] = action.GetArguments<2>();

  ScTemplate asked_questions_template;

  asked_questions_template.Quintuple(
      user,
      ScType::VarCommonArc,
      ScType::VarNode >> "_asked_questions",
      ScType::VarPermPosArc,
      ChoiceNextQuestionKeynodes::nrel_asked_questions);

  asked_questions_template.Triple("_asked_questions", ScType::VarPermPosArc, ScType::VarNode >> "_asked_question");

  ScTemplate all_questions_template;

  all_questions_template.Triple(test, ScType::VarPermPosArc, ScType::VarNode >> "_question");

  ScTemplateSearchResult asked_questions_search_result;

  if (m_context.SearchByTemplate(asked_questions_template, asked_questions_search_result)
      && asked_questions_search_result.Size() > 0)
  {
    ScTemplateSearchResult all_questions_search_result;
    m_context.SearchByTemplate(all_questions_template, all_questions_search_result);

    if (asked_questions_search_result.Size() == all_questions_search_result.Size())
    {
      ScAddr updateRatingAction = m_context.GenerateNode(ScType::ConstNodeClass);
      m_context.GenerateConnector(
          ScType::ConstPermPosArc, RatingUpdateKeynodes::action_rating_update, updateRatingAction);

      ScAddr argsSet_1 = m_context.GenerateNode(ScType::ConstNodeTuple);
      m_context.GenerateConnector(ScType::ConstPermPosArc, argsSet_1, user);
      m_context.GenerateConnector(ScType::ConstPermPosArc, argsSet_1, test);
      m_context.GenerateConnector(ScType::ConstPermPosArc, updateRatingAction, argsSet_1);

      ScAction ratingAction = m_context.ConvertToAction(updateRatingAction);
      ratingAction.Initiate();

      ScAddr deleteOldNodesAction = m_context.GenerateNode(ScType::ConstNodeClass);
      m_context.GenerateConnector(
          ScType::ConstPermPosArc, DeleteOldNodesKeynodes::action_delete_old_nodes, deleteOldNodesAction);

      ScAddr argsSet_2 = m_context.GenerateNode(ScType::ConstNodeTuple);
      m_context.GenerateConnector(ScType::ConstPermPosArc, argsSet_2, user);
      m_context.GenerateConnector(ScType::ConstPermPosArc, deleteOldNodesAction, argsSet_2);

      ScAction deleteAction = m_context.ConvertToAction(deleteOldNodesAction);
      deleteAction.Initiate();

      return action.FinishSuccessfully();
    }
    else if (asked_questions_search_result.Size() < all_questions_search_result.Size())
    {
      ScAddr next_question = FindNextUnaskedQuestion(asked_questions_search_result, all_questions_search_result);

      if (next_question.IsValid())
      {
        ScTemplateResultItem asked_questions_item;
        asked_questions_search_result.Get(0, asked_questions_item);

        ScAddr asked_questions;
        asked_questions_item.Get("_asked_questions", asked_questions);
        m_context.GenerateConnector(ScType::ConstPermPosArc, asked_questions, next_question);

        action.SetResult(next_question);
        return action.FinishSuccessfully();
      }
      else
      {
        return action.FinishUnsuccessfully();
      }
    }
    else
    {
      return action.FinishUnsuccessfully();
    }
  }
  else
  {
    ScAddr asked_questions = m_context.GenerateNode(ScType::ConstNodeTuple);

    ScAddr asked_questions_connector = m_context.GenerateConnector(ScType::ConstCommonArc, user, asked_questions);
    m_context.GenerateConnector(
        ScType::ConstPermPosArc, ChoiceNextQuestionKeynodes::nrel_asked_questions, asked_questions_connector);

    ScTemplateSearchResult all_questions_search_result;
    m_context.SearchByTemplate(all_questions_template, all_questions_search_result);

    ScAddr next_question = FindNextUnaskedQuestion(asked_questions_search_result, all_questions_search_result);

    m_context.GenerateConnector(ScType::ConstPermPosArc, asked_questions, next_question);

    action.SetResult(next_question);
    return action.FinishSuccessfully();
  }
}

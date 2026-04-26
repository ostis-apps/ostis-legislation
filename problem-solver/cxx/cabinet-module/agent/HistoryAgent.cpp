#include "HistoryAgent.hpp"

#include "keynodes/CabinetKeynodes.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace utils;

ScAddr ScAddHistoryEntryAgent::GetActionClass() const
{
  // action_add_history_entry должен быть объявлен в CabinetKeynodes
  return CabinetKeynodes::action_add_history_entry;
}

ScResult ScAddHistoryEntryAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("ScAddHistoryEntryAgent started");
  ScStructure result = m_context.GenerateStructure();

  try
  {
    ScTemplate tpl;
    ScTemplateSearchResult res;

    // rrel_user
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_user",
      ScType::VarNode >> "_user");
    tpl.Triple(
      CabinetKeynodes::rrel_user,
      ScType::VarPermPosArc,
      "_arc_user");

    if (!m_context.SearchByTemplate(tpl, res) || res.IsEmpty())
    {
      create_error_result("User is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }
    ScAddr user_node = res[0]["_user"];

    // rrel_text
    tpl.Clear();
    res.Clear();
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_text",
      ScType::VarNodeLink >> "_text");
    tpl.Triple(
      CabinetKeynodes::rrel_text,
      ScType::VarPermPosArc,
      "_arc_text");

    if (!m_context.SearchByTemplate(tpl, res) || res.IsEmpty())
    {
      create_error_result("Text is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }
    ScAddr text_link = res[0]["_text"];

    // rrel_date (опционально)
    tpl.Clear();
    res.Clear();
    ScAddr date_link;
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_date",
      ScType::VarNodeLink >> "_date");
    tpl.Triple(
      CabinetKeynodes::rrel_date,
      ScType::VarPermPosArc,
      "_arc_date");
    if (m_context.SearchByTemplate(tpl, res) && !res.IsEmpty())
      date_link = res[0]["_date"];

    // rrel_article (опционально)
    tpl.Clear();
    res.Clear();
    ScAddr article_node;
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_article",
      ScType::VarNode >> "_article");
    tpl.Triple(
      CabinetKeynodes::rrel_article,
      ScType::VarPermPosArc,
      "_arc_article");
    if (m_context.SearchByTemplate(tpl, res) && !res.IsEmpty())
      article_node = res[0]["_article"];

    if (!create_history_entry(user_node, text_link, date_link, article_node, result))
    {
      create_error_result("Failed to create history entry", result);
      action.SetResult(result);
      return action.FinishWithError();
    }

    action.SetResult(result);
    return action.FinishSuccessfully();
  }
  catch (ScException const & ex)
  {
    SC_LOG_ERROR(ex.Message());
    create_error_result("Internal error in history agent", result);
    action.SetResult(result);
    return action.FinishWithError();
  }
}

bool ScAddHistoryEntryAgent::create_history_entry(
  ScAddr const & user_node,
  ScAddr const & text_link,
  ScAddr const & date_link,
  ScAddr const & article_node,
  ScStructure & result)
{
  ScAddr history = m_context.GenerateNode(ScType::ConstNode);
  if (!history.IsValid())
    return false;

  // тип
  m_context.GenerateConnector(ScType::ConstPermPosArc,
                              CabinetKeynodes::concept_user_query,
                              history);

  // user -- nrel_query_history --> history
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, user_node, history);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_query_history,
                                common);
  }

  // history -- nrel_query_text --> text_link
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, history, text_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_query_text,
                                common);
  }

  // history -- nrel_query_timestamp --> date_link
  if (date_link.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, history, date_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_query_timestamp,
                                common);
  }

  // history -- nrel_viewed_article --> article_node
  if (article_node.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, history, article_node);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_viewed_article,
                                common);
  }

  create_success_result(history, result);
  return true;
}

void ScAddHistoryEntryAgent::create_success_result(ScAddr const & history_node, ScStructure & result)
{
  result << history_node;
}

void ScAddHistoryEntryAgent::create_error_result(std::string const & message, ScStructure & result)
{
  ScAddr err = m_context.GenerateLink();
  m_context.SetLinkContent(err, message);
  result << err;
}
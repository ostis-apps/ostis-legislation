#include "NotesAgent.hpp"

#include "keynodes/CabinetKeynodes.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace utils;

ScAddr ScAddNoteAgent::GetActionClass() const
{
  return CabinetKeynodes::action_add_note;
}

ScResult ScAddNoteAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("ScAddNoteAgent started");
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

    // rrel_article
    tpl.Clear();
    res.Clear();
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_article",
      ScType::VarNode >> "_article");
    tpl.Triple(
      CabinetKeynodes::rrel_article,
      ScType::VarPermPosArc,
      "_arc_article");
    if (!m_context.SearchByTemplate(tpl, res) || res.IsEmpty())
    {
      create_error_result("Article is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }
    ScAddr article_node = res[0]["_article"];

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

    // rrel_created (опционально)
    tpl.Clear();
    res.Clear();
    ScAddr created_link;
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_created",
      ScType::VarNodeLink >> "_created");
    tpl.Triple(
      CabinetKeynodes::rrel_created,
      ScType::VarPermPosArc,
      "_arc_created");
    if (m_context.SearchByTemplate(tpl, res) && !res.IsEmpty())
      created_link = res[0]["_created"];

    // rrel_updated (опционально)
    tpl.Clear();
    res.Clear();
    ScAddr updated_link;
    tpl.Triple(
      action,
      ScType::VarPermPosArc >> "_arc_updated",
      ScType::VarNodeLink >> "_updated");
    tpl.Triple(
      CabinetKeynodes::rrel_updated,
      ScType::VarPermPosArc,
      "_arc_updated");
    if (m_context.SearchByTemplate(tpl, res) && !res.IsEmpty())
      updated_link = res[0]["_updated"];

    if (!create_note(user_node, article_node, text_link, created_link, updated_link, result))
    {
      create_error_result("Failed to create note", result);
      action.SetResult(result);
      return action.FinishWithError();
    }

    action.SetResult(result);
    return action.FinishSuccessfully();
  }
  catch (ScException const & ex)
  {
    SC_LOG_ERROR(ex.Message());
    create_error_result("Internal error in note agent", result);
    action.SetResult(result);
    return action.FinishWithError();
  }
}

bool ScAddNoteAgent::create_note(
  ScAddr const & user_node,
  ScAddr const & article_node,
  ScAddr const & text_link,
  ScAddr const & created_link,
  ScAddr const & updated_link,
  ScStructure & result)
{
  ScAddr note = m_context.GenerateNode(ScType::ConstNode);
  if (!note.IsValid())
    return false;

  // тип
  m_context.GenerateConnector(ScType::ConstPermPosArc,
                              CabinetKeynodes::concept_user_note,
                              note);

  // user -- nrel_user_notes --> note
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, user_node, note);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_user_notes,
                                common);
  }

  // note -- nrel_note_article --> article
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, note, article_node);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_note_article,
                                common);
  }

  // note -- nrel_note_text --> text_link
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, note, text_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_note_text,
                                common);
  }

  // created
  if (created_link.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, note, created_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_note_created,
                                common);
  }

  // updated
  if (updated_link.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, note, updated_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_note_updated,
                                common);
  }

  create_success_result(note, result);
  return true;
}

void ScAddNoteAgent::create_success_result(ScAddr const & note_node, ScStructure & result)
{
  result << note_node;
}

void ScAddNoteAgent::create_error_result(std::string const & message, ScStructure & result)
{
  ScAddr err = m_context.GenerateLink();
  m_context.SetLinkContent(err, message);
  result << err;
}
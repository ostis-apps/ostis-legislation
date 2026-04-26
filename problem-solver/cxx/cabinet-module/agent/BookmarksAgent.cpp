#include "BookmarksAgent.hpp"

#include "keynodes/CabinetKeynodes.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace utils;

std::string ScAddBookmarkAgent::readStringFromLink(ScAddr const & linkAddr)
{
  ScStreamPtr stream = m_context.GetLinkContent(linkAddr);
  if (!stream || stream->Size() == 0)
    return "";

  std::string result;
  result.resize(stream->Size());
  size_t readBytes = 0;
  stream->Read(&result[0], stream->Size(), readBytes);
  return result;
}

ScAddr ScAddBookmarkAgent::GetActionClass() const
{
  return CabinetKeynodes::action_add_bookmark;
}

ScResult ScAddBookmarkAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("ScAddBookmarkAgent started");

  ScStructure result = m_context.GenerateStructure();

  try
  {
    ScTemplate argTemplate;
    ScTemplateSearchResult searchResult;

    // rrel_user — пользователь
    argTemplate.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNode >> "_user");
    argTemplate.Triple(
      CabinetKeynodes::rrel_user,
      ScType::VarPermPosArc,
      "_arc");

    if (!m_context.SearchByTemplate(argTemplate, searchResult) || searchResult.IsEmpty())
    {
      SC_LOG_ERROR("User argument not found");
      create_error_result("User is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    ScAddr user_node = searchResult[0]["_user"];

    // rrel_article — статья
    argTemplate.Clear();
    searchResult.Clear();

    argTemplate.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNode >> "_article");
    argTemplate.Triple(
      CabinetKeynodes::rrel_article,
      ScType::VarPermPosArc,
      "_arc");

    if (!m_context.SearchByTemplate(argTemplate, searchResult) || searchResult.IsEmpty())
    {
      SC_LOG_ERROR("Article argument not found");
      create_error_result("Article is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    ScAddr article_node = searchResult[0]["_article"];

    // rrel_tags — link с тегами (опционально)
    ScAddr tags_link;
    argTemplate.Clear();
    searchResult.Clear();

    argTemplate.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNodeLink >> "_tags");
    argTemplate.Triple(
      CabinetKeynodes::rrel_tags,
      ScType::VarPermPosArc,
      "_arc");

    if (m_context.SearchByTemplate(argTemplate, searchResult) && !searchResult.IsEmpty())
      tags_link = searchResult[0]["_tags"];

    // rrel_date — дата (опционально)
    ScAddr date_link;
    argTemplate.Clear();
    searchResult.Clear();

    argTemplate.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNodeLink >> "_date");
    argTemplate.Triple(
      CabinetKeynodes::rrel_date,
      ScType::VarPermPosArc,
      "_arc");

    if (m_context.SearchByTemplate(argTemplate, searchResult) && !searchResult.IsEmpty())
      date_link = searchResult[0]["_date"];

    if (!create_bookmark(user_node, article_node, tags_link, date_link, result))
    {
      create_error_result("Failed to create bookmark", result);
      action.SetResult(result);
      return action.FinishWithError();
    }

    action.SetResult(result);
    return action.FinishSuccessfully();
  }
  catch (ScException const & ex)
  {
    SC_LOG_ERROR(ex.Message());
    create_error_result("Internal error in bookmark agent", result);
    action.SetResult(result);
    return action.FinishWithError();
  }
}

bool ScAddBookmarkAgent::create_bookmark(
  ScAddr const & user_node,
  ScAddr const & article_node,
  ScAddr const & tags_link,
  ScAddr const & date_link,
  ScStructure & result)
{
  ScAddr bookmark = m_context.GenerateNode(ScType::ConstNode);
  if (!bookmark.IsValid())
    return false;

  // тип
  m_context.GenerateConnector(ScType::ConstPermPosArc,
                              CabinetKeynodes::concept_bookmark,
                              bookmark);

  // user -- nrel_user_bookmarks --> bookmark
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, user_node, bookmark);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_user_bookmarks,
                                common);
  }

  // bookmark -- nrel_bookmark_article --> article
  if (article_node.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, bookmark, article_node);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_bookmark_article,
                                common);
  }

  // теги (если есть link)
  if (tags_link.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, bookmark, tags_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_bookmark_tags,
                                common);
  }

  // дата (если есть link)
  if (date_link.IsValid())
  {
    ScAddr common = m_context.GenerateConnector(ScType::ConstCommonArc, bookmark, date_link);
    m_context.GenerateConnector(ScType::ConstPermPosArc,
                                CabinetKeynodes::nrel_bookmark_date,
                                common);
  }

  create_success_result(bookmark, result);
  return true;
}

void ScAddBookmarkAgent::create_success_result(ScAddr const & bookmark_node, ScStructure & result)
{
  // как в ScRegistrationAgent: можно положить bookmark_node в структуру результата
  result << bookmark_node;
}

void ScAddBookmarkAgent::create_error_result(std::string const & message, ScStructure & result)
{
  ScAddr error_link = m_context.GenerateLink();
  m_context.SetLinkContent(error_link, message);
  result << error_link;
}
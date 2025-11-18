#include "RatingUpdateAgent.hpp"
#include "keynodes/RatingUpdateKeynodes.hpp"

using namespace utils;

ScResult RatingUpdateAgent::DoProgram(ScAction & action)
{
  auto const & [test, user] = action.GetArguments<2>();

  ScTemplate tier_template;

  tier_template.Quintuple(
      user, ScType::VarCommonArc, ScType::VarNode >> "_tier", ScType::VarPermPosArc, RatingUpdateKeynodes::nrel_tier);

  tier_template.Quintuple(
      "_tier",
      ScType::VarCommonArc,
      ScType::VarNode >> "_theme",
      ScType::VarPermPosArc,
      RatingUpdateKeynodes::nrel_theme_tier);

  tier_template.Triple("_theme", ScType::VarPermPosArc, test);

  tier_template.Triple("_concept_tier", ScType::VarPermPosArc >> "_concept_tier_edge", "_tier");

  ScTemplateSearchResult tier_search_result;

  if (m_context.SearchByTemplate(tier_template, tier_search_result) && tier_search_result.Size() > 0)
  {
    ScTemplateResultItem tier_item;
    tier_search_result.Get(0, tier_item);

    ScAddr concept_tier;
    tier_item.Get("_concept_tier", concept_tier);

    ScAddr concept_tier_edge;
    tier_item.Get("_concept_tier_edge", concept_tier_edge);

    ScAddr tier;
    tier_item.Get("_tier", tier);

    if (concept_tier == RatingUpdateKeynodes::concept_first_tier)
    {
      m_context.EraseElement(concept_tier_edge);

      m_context.GenerateConnector(ScType::PermPosArc, RatingUpdateKeynodes::concept_second_tier, tier);
      return action.FinishSuccessfully();
    }
    else if (concept_tier == RatingUpdateKeynodes::concept_second_tier)
    {
      m_context.EraseElement(concept_tier_edge);

      m_context.GenerateConnector(ScType::ConstPermPosArc, RatingUpdateKeynodes::concept_third_tier, tier);
      return action.FinishSuccessfully();
    }
    else
    {
      return action.FinishUnsuccessfully();
    }
  }
  else
  {
    ScAddr tier = m_context.GenerateNode(ScType::ConstNode);

    ScAddr user_tier_connector = m_context.GenerateConnector(ScType::ConstCommonArc, user, tier);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RatingUpdateKeynodes::nrel_tier, user_tier_connector);

    m_context.GenerateConnector(ScType::ConstPermPosArc, RatingUpdateKeynodes::concept_first_tier, tier);

    ScTemplate theme_test_template;
    theme_test_template.Triple("_theme", ScType::VarPermPosArc, test);

    ScTemplateSearchResult theme_test_search_result;

    if (m_context.SearchByTemplate(theme_test_template, theme_test_search_result)
        && theme_test_search_result.Size() > 0)
    {
      ScTemplateResultItem theme_tier_item;
      theme_test_search_result.Get(0, theme_tier_item);

      ScAddr theme_tier;
      theme_tier_item.Get("_theme", theme_tier);

      ScAddr theme_tier_connector = m_context.GenerateConnector(ScType::ConstCommonArc, tier, theme_tier);
      m_context.GenerateConnector(ScType::ConstPermPosArc, RatingUpdateKeynodes::nrel_theme_tier, theme_tier_connector);
      return action.FinishSuccessfully();
    }
    else
    {
      return action.FinishUnsuccessfully();
    }
  }
}

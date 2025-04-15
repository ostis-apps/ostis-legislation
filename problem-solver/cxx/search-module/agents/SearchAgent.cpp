#include "SearchAgent.hpp"
#include "keynodes/SearchKeynodes.hpp"

bool isSubstring(const std::string& textToSearch, const std::string& fullText) {
  if (textToSearch.empty()) {
    return true;
  }

  if (fullText.length() < textToSearch.length()) {
    return false;
  }

  return fullText.find(textToSearch) != std::string::npos;
}

void generateCommonTemplateElementsForTitle(ScTemplate& inputTemplate) {
  inputTemplate.Quintuple(
      ScType::VarNode >> "_concept",
      ScType::VarCommonArc,
      ScType::VarNodeLink >> "_title",
      ScType::VarPermPosArc,
      SearchKeynodes::NREL_MAIN_IDTF
  );
}

void generateCommonTemplateElementsForBody(ScTemplate& inputTemplate) {
  inputTemplate.Quintuple(
      ScType::VarNode >> "_1",
      ScType::VarPermPosArc,
      ScType::VarNode >> "_concept",
      ScType::VarPermPosArc,
      SearchKeynodes::RREL_KEY_SC_ELEMENT
  );

  inputTemplate.Quintuple(
      ScType::VarNode >> "_2",
      ScType::VarCommonArc,
      "_1",
      ScType::VarPermPosArc,
      SearchKeynodes::NREL_SC_TEXT_TRANSLATION
  );

  inputTemplate.Quintuple(
      "_2",
      ScType::VarPermPosArc,
      ScType::VarNodeLink >> "_body",
      ScType::VarPermPosArc,
      SearchKeynodes::RREL_EXAMPLE
  );
}

ScAddr SearchAgent::GetActionClass() const
{
  return SearchKeynodes::action_search;
}

ScResult SearchAgent::DoProgram(ScAction & action)
{
  auto const & [partOfConceptToSearchAddr, searchAreaAddr, textToSearchAddr] = action.GetArguments<3>();
  int resultCount = 0;

  std::string textToSearch;
  m_context.GetLinkContent(textToSearchAddr, textToSearch);

  SC_LOG_INFO("Часть для поиска: " + m_context.GetElementSystemIdentifier(partOfConceptToSearchAddr));
  SC_LOG_INFO("Область поиска: " + m_context.GetElementSystemIdentifier(searchAreaAddr));
  SC_LOG_INFO("Текст для поиска: " + textToSearch);

  ScTemplate templateToFindConceptsTitle;
  ScTemplate templateToFindConceptsBody;
  ScTemplate templateToFindArticlesTitle;
  ScTemplate templateToFindArticlesBody;

  generateCommonTemplateElementsForBody(templateToFindConceptsBody);
  generateCommonTemplateElementsForBody(templateToFindArticlesBody);
  generateCommonTemplateElementsForTitle(templateToFindConceptsTitle);
  generateCommonTemplateElementsForTitle(templateToFindArticlesTitle);

  ScStructure resultStructure = m_context.GenerateStructure();

  // Поиск для статей
  if (searchAreaAddr == SearchKeynodes::ARTICLES_SEARCH || searchAreaAddr == SearchKeynodes::FULL_SEARCH) {
    templateToFindArticlesTitle.Triple(
        SearchKeynodes::BELARUS_LEGAL_ARTICLE,
        ScType::VarPermPosArc,
        "_concept"
        );

    templateToFindArticlesBody.Triple(
        SearchKeynodes::BELARUS_LEGAL_ARTICLE,
        ScType::VarPermPosArc,
        "_concept"
    );

    // Поиск по телу
    if (partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_BODY_SEARCH || partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_FULL_SEARCH) {
      ScTemplateSearchResult templateToFindArticlesBodySearchResult;
      m_context.SearchByTemplate(templateToFindArticlesBody, templateToFindArticlesBodySearchResult);
      for (int i = 0; i < templateToFindArticlesBodySearchResult.Size(); i++) {
        ScTemplateResultItem templateToFindArticlesBodyResultItem;
        templateToFindArticlesBodySearchResult.Get(i, templateToFindArticlesBodyResultItem);

        ScAddr conceptOfFoundArticle;
        ScAddr bodyTextAddr;
        templateToFindArticlesBodyResultItem.Get("_concept", conceptOfFoundArticle);
        templateToFindArticlesBodyResultItem.Get("_body", bodyTextAddr);

        std::string bodyText;
        m_context.GetLinkContent(bodyTextAddr, bodyText);

        if (isSubstring(textToSearch, bodyText)) {
          resultCount++;
          SC_LOG_INFO("Идентификатор найденного понятия: " + m_context.GetElementSystemIdentifier(conceptOfFoundArticle));
          SC_LOG_INFO("Тело найденного понятия: " + bodyText);
          resultStructure.Append(conceptOfFoundArticle);
        }
      }
    }

    // Поиск по заголовку
    if (partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_TITLE_SEARCH || partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_FULL_SEARCH) {
      ScTemplateSearchResult templateToFindArticlesTitleSearchResult;
      m_context.SearchByTemplate(templateToFindArticlesTitle, templateToFindArticlesTitleSearchResult);
      for (int i = 0; i < templateToFindArticlesTitleSearchResult.Size(); i++) {
        ScTemplateResultItem templateToFindArticlesTitleResultItem;
        templateToFindArticlesTitleSearchResult.Get(i, templateToFindArticlesTitleResultItem);

        ScAddr conceptOfFoundArticle;
        ScAddr titleTextAddr;
        templateToFindArticlesTitleResultItem.Get("_concept", conceptOfFoundArticle);
        templateToFindArticlesTitleResultItem.Get("_title", titleTextAddr);

        std::string titleText;
        m_context.GetLinkContent(titleTextAddr, titleText);

        if (isSubstring(textToSearch, titleText)) {
          resultCount++;
          SC_LOG_INFO("Идентификатор найденного понятия: " + m_context.GetElementSystemIdentifier(conceptOfFoundArticle));
          SC_LOG_INFO("Заголовок найденного понятия: " + titleText);
          resultStructure.Append(conceptOfFoundArticle);
        }
      }
    }
  }

  // Поиск для понятий
  if (searchAreaAddr == SearchKeynodes::CONCEPTS_SEARCH || searchAreaAddr == SearchKeynodes::FULL_SEARCH) {
    templateToFindConceptsTitle.Triple(
        SearchKeynodes::BELARUS_LEGAL_TERM,
        ScType::VarPermPosArc,
        "_concept"
    );

    templateToFindConceptsBody.Triple(
        SearchKeynodes::BELARUS_LEGAL_TERM,
        ScType::VarPermPosArc,
        "_concept"
    );

    // Поиск по телу
    if (partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_BODY_SEARCH || partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_FULL_SEARCH) {
      ScTemplateSearchResult templateToFindConceptsBodySearchResult;
      m_context.SearchByTemplate(templateToFindConceptsBody, templateToFindConceptsBodySearchResult);
      for (int i = 0; i < templateToFindConceptsBodySearchResult.Size(); i++) {
        ScTemplateResultItem templateToFindArticlesBodyResultItem;
        templateToFindConceptsBodySearchResult.Get(i, templateToFindArticlesBodyResultItem);

        ScAddr conceptOfFoundConcept;
        ScAddr bodyTextAddr;
        templateToFindArticlesBodyResultItem.Get("_concept", conceptOfFoundConcept);
        templateToFindArticlesBodyResultItem.Get("_body", bodyTextAddr);

        std::string bodyText;
        m_context.GetLinkContent(bodyTextAddr, bodyText);

        if (isSubstring(textToSearch, bodyText)) {
          resultCount++;
          SC_LOG_INFO("Идентификатор найденного понятия: " + m_context.GetElementSystemIdentifier(conceptOfFoundConcept));
          SC_LOG_INFO("Тело найденного понятия: " + bodyText);
          resultStructure.Append(conceptOfFoundConcept);
        }
      }
    }

    // Поиск по заголовку
    if (partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_TITLE_SEARCH || partOfConceptToSearchAddr == SearchKeynodes::CONCEPT_FULL_SEARCH) {
      ScTemplateSearchResult templateToFindConceptsTitleSearchResult;
      m_context.SearchByTemplate(templateToFindConceptsTitle, templateToFindConceptsTitleSearchResult);
      for (int i = 0; i < templateToFindConceptsTitleSearchResult.Size(); i++) {
        ScTemplateResultItem templateToFindConceptsTitleResultItem;
        templateToFindConceptsTitleSearchResult.Get(i, templateToFindConceptsTitleResultItem);

        ScAddr conceptOfFoundConcept;
        ScAddr titleTextAddr;
        templateToFindConceptsTitleResultItem.Get("_concept", conceptOfFoundConcept);
        templateToFindConceptsTitleResultItem.Get("_title", titleTextAddr);

        std::string titleText;
        m_context.GetLinkContent(titleTextAddr, titleText);

        if (isSubstring(textToSearch, titleText)) {
          resultCount++;
          SC_LOG_INFO("Идентификатор найденного понятия: " + m_context.GetElementSystemIdentifier(conceptOfFoundConcept));
          SC_LOG_INFO("Заголовок найденного понятия: " + titleText);
          resultStructure.Append(conceptOfFoundConcept);
        }
      }
    }
  }

  SC_LOG_INFO("Найденных понятий: " + std::to_string(resultCount));
  action.SetResult(resultStructure);
  return action.FinishSuccessfully();
}


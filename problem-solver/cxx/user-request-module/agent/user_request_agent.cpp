#include "user_request_agent.hpp"
#include <string>
#include <sc-memory/sc_memory_headers.hpp>
#include "keynodes/user_request_keynodes.hpp"

ScAddr ScUserRequestAgent::GetActionClass() const
{
    return UserRequestKeynodes::action_user_request;
}

std::vector<std::string> LemmaProcessing(const std::string &input)
{ 
  std::vector<std::string> words;
    std::istringstream stream(input);
    std::string word;
    
    while (stream >> word) {
        words.push_back(word);
        SC_LOG_INFO(word);
    }
    
    return words;
}

std::string getFirstWord(const std::string& input) {
  std::istringstream stream(input);
  std::string firstWord;
  stream >> firstWord;
  return firstWord;
}

bool isSubstring(const std::string& textToSearch, const std::string& fullText) {
  if (textToSearch.empty()) {
    return true;
  }

  if (fullText.length() < textToSearch.length()) {
    return false;
  }

  return fullText.find(textToSearch) != std::string::npos;
}

bool areEqual(const std::string& str1, const std::string& str2) {
  return str1 == str2;
}

void generateCommonTemplateFromTitle(ScTemplate& inputTemplate) {
  inputTemplate.Quintuple(
      ScType::VarNode >> "_concept",
      ScType::VarCommonArc,
      ScType::VarNodeLink >> "_title",
      ScType::VarPermPosArc,
      UserRequestKeynodes::nrel_main_idtf
  );

  inputTemplate.Triple(
      UserRequestKeynodes::belarus_legal_term,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_concept"
  );
};

void generateCommonTemplateFromBody(ScTemplate& inputTemplate) {
  inputTemplate.Quintuple(
      ScType::VarNode >> "_1",
      ScType::VarPermPosArc,
      ScType::VarNode >> "_concept",
      ScType::VarPermPosArc,
      UserRequestKeynodes::rrel_key_sc_element
  );

  inputTemplate.Quintuple(
      ScType::VarNode >> "_2",
      ScType::VarCommonArc,
      "_1",
      ScType::VarPermPosArc,
      UserRequestKeynodes::nrel_sc_text_translation
  );

  inputTemplate.Quintuple(
      "_2",
      ScType::VarPermPosArc,
      ScType::VarNodeLink >> "_body",
      ScType::VarPermPosArc,
      UserRequestKeynodes::rrel_example
  );
}

void applyLangParams(ScTemplate& inputTemplateTitle, ScTemplate& inputTemplateText, const ScKeynode lang_node)
{
  inputTemplateTitle.Triple(
    lang_node,
    ScType::VarPermPosArc,
    ScType::VarNodeLink >> "_title"
);
  inputTemplateText.Triple(
    lang_node,
    ScType::VarPermPosArc,
    ScType::VarNodeLink >> "_body"
);
}

ScResult ScUserRequestAgent::DoProgram(ScAction & action)
{
  auto const & [requestAddr] = action.GetArguments<1>(); 
  int count = 0;
  std::string stringContent;
  bool const stringContentExist = m_context.GetLinkContent(requestAddr, stringContent);

  SC_LOG_INFO(stringContent);

  std::vector<std::string> lemma_vec = LemmaProcessing(stringContent);

  for (auto item : lemma_vec)
  { 
    SC_LOG_INFO(item);
    ScAddrSet const & linkAddrs1 = m_context.SearchLinksByContent(item);
    if(linkAddrs1.size() > 0)
    {
      stringContent = item;
      SC_LOG_INFO("SCLINK NOW" + item);
    }

  }

  ScTemplate FindTemplateForTitle;
  ScTemplate FindTemplateForText;

  ScTemplate FindTemplate;

  ScStructure resultStruct = m_context.GenerateStructure();

  generateCommonTemplateFromTitle(FindTemplate);
  generateCommonTemplateFromBody(FindTemplate);
  applyLangParams(FindTemplate, FindTemplate, UserRequestKeynodes::lang_ru);

  // SC_LOG_INFO("For title empty?" << FindTemplateForTitle.IsEmpty());
  // SC_LOG_INFO("Title temp size" << std::to_string(FindTemplateForTitle.Size()));

  // SC_LOG_INFO("For text empty?" << FindTemplateForText.IsEmpty());
  // SC_LOG_INFO("Text temp size" << std::to_string(FindTemplateForText.Size()));
  SC_LOG_INFO("Text temp size" << std::to_string(FindTemplate.Size()));

  ScTemplateSearchResult FindTemplateSearchResult;
  m_context.SearchByTemplate(FindTemplate, FindTemplateSearchResult);
  for (int i = 0; i < FindTemplateSearchResult.Size(); i++) {
    ScTemplateResultItem FindTemplateSearchResultItem;
    FindTemplateSearchResult.Get(i, FindTemplateSearchResultItem);

    ScAddr conceptOfFoundArticle;
    ScAddr bodyTextAddr;
    ScAddr titleTextAddr;
    FindTemplateSearchResultItem.Get("_concept", conceptOfFoundArticle);
    FindTemplateSearchResultItem.Get("_body", bodyTextAddr);
    FindTemplateSearchResultItem.Get("_title", titleTextAddr);
    // SC_LOG_INFO(m_context.GetElementType(conceptOfFoundArticle));
    // SC_LOG_INFO(m_context.GetElementType(bodyTextAddr));
    // SC_LOG_INFO(m_context.GetElementType(titleTextAddr));
    

    std::string bodyText;
    std::string titleText;
    m_context.GetLinkContent(bodyTextAddr, bodyText);
    m_context.GetLinkContent(titleTextAddr, titleText);
    
    // SC_LOG_INFO(m_context.GetElementSystemIdentifier(conceptOfFoundArticle));
    // SC_LOG_INFO(bodyText);
    // SC_LOG_INFO(titleText);

    bodyText = getFirstWord(bodyText);
    if ((areEqual(stringContent, titleText)) && (areEqual(stringContent, bodyText)))
    {
      count++;
      // SC_LOG_INFO("title: " + titleText);
      // SC_LOG_INFO("idtf: " + m_context.GetElementSystemIdentifier(conceptOfFoundArticle));
      // SC_LOG_INFO("body: " + bodyText);
      resultStruct.Append(bodyTextAddr);
    }
  }

  SC_LOG_INFO("total" + std::to_string(count));
  action.SetResult(resultStruct);
  return action.FinishSuccessfully();
}
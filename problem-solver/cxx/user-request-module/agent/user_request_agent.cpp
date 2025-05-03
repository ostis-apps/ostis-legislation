#include "user_request_agent.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <regex>
#include <sc-memory/sc_memory_headers.hpp>
#include "keynodes/user_request_keynodes.hpp"

ScAddr ScUserRequestAgent::GetActionClass() const
{
    return UserRequestKeynodes::action_user_request;
}

std::string getFirstWord(const std::string& input) {
  std::istringstream stream(input);
  std::string firstWord;
  stream >> firstWord;
  return firstWord;
}

std::string str_tolower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), 
                [](unsigned char c){ return std::tolower(c); });
  return str;
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

void generateCommonTemplate(ScTemplate& inputTemplate, ScAddr& item) {
  inputTemplate.Quintuple(
    ScType::VarNode >> "_main_node",
    ScType::VarCommonArc,
    item >> "_title_link",
    ScType::VarPermPosArc,
    UserRequestKeynodes::nrel_main_idtf
  );

  inputTemplate.Quintuple(
    ScType::NodeVarClass >> "_concept",
    ScType::EdgeDCommonVar,
    "_main_node",
    ScType::VarPermPosArc,
    UserRequestKeynodes::nrel_related_concept
  );

  inputTemplate.Quintuple(
    "_main_node",
    ScType::EdgeDCommonVar,
    ScType::NodeVar >> "_article",
    ScType::VarPermPosArc,
    UserRequestKeynodes::nrel_related_article
  );

  inputTemplate.Triple(
    UserRequestKeynodes::lang_ru,
    ScType::VarPermPosArc,
    "_title_link"
  );

  inputTemplate.Triple(
    UserRequestKeynodes::belarus_legal_term,
    ScType::VarPermPosArc,
    "_main_node"
  );

  inputTemplate.Quintuple(
    ScType::VarNode >> "_1",
    ScType::VarPermPosArc,
    "_main_node",
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
    ScType::VarNodeLink >> "_def_link",
    ScType::VarPermPosArc,
    UserRequestKeynodes::rrel_example
  );

  inputTemplate.Triple(
    UserRequestKeynodes::lang_ru,
    ScType::VarPermPosArc,
    "_def_link"
  );
  
};

ScResult ScUserRequestAgent::DoProgram(ScAction & action)
{
  auto const & [requestAddr] = action.GetArguments<1>(); 
  
  std::string stringContent;
  
  bool const stringContentExist = m_context.GetLinkContent(requestAddr, stringContent);

  ScAddrSet const & linkAddrs1 = m_context.SearchLinksByContent(stringContent);
  
  ScStructure resultStruct = m_context.GenerateStructure();
  
  if (linkAddrs1.size() > 1)
  {
    SC_LOG_INFO(linkAddrs1.size());
    for (auto item : linkAddrs1)
    {
      ScTemplate FindTemplate;
    
      generateCommonTemplate(FindTemplate, item);
    
      ScTemplateSearchResult FindTemplateSearchResult;
    
      m_context.SearchByTemplate(FindTemplate, FindTemplateSearchResult);
    
      for (int i = 0; i < FindTemplateSearchResult.Size(); i++) {
        ScTemplateResultItem FindTemplateSearchResultItem;
    
        FindTemplateSearchResult.Get(i, FindTemplateSearchResultItem);
    
        ScAddr titleTextAddr;
        ScAddr defTextAddr;
        ScAddr conceptAddr;
        ScAddr articleAddr;

        FindTemplateSearchResultItem.Get("_title_link", titleTextAddr);
        
        FindTemplateSearchResultItem.Get("_def_link", defTextAddr); 
        
        FindTemplateSearchResultItem.Get("_concept", conceptAddr);
        
        FindTemplateSearchResultItem.Get("_article", articleAddr);
    
        std::string titleText;
        std::string defText;
    
        m_context.GetLinkContent(titleTextAddr, titleText);
        m_context.GetLinkContent(defTextAddr, defText);
    
        defText = getFirstWord(defText);
        
        resultStruct.Append(defTextAddr);
        resultStruct.Append(conceptAddr);
        resultStruct.Append(articleAddr);
      }
    }
  }
  else
  {
    return action.FinishUnsuccessfully();
  }

  action.SetResult(resultStruct);
  return action.FinishSuccessfully();
}

#pragma once

#include <sc-memory/sc_agent.hpp>

class ChoiceNextQuestionAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
  bool IsQuestionAsked(ScTemplateSearchResult const & asked_questions_search_result, std::string id_of_question);
  ScAddr FindNextUnaskedQuestion(
      ScTemplateSearchResult const & asked_questions_search_result,
      ScTemplateSearchResult const & all_questions_search_result);
};

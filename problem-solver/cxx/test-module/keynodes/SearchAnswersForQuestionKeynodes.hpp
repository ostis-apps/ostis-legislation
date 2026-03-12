#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_memory.hpp>

namespace utils
{

class SearchAnswersForQuestionKeynodes : public ScObject
{
public:
  static inline ScAddr action_search_answers_for_question;
  static inline ScAddr nrel_answer;

  static void Initialize(ScMemoryContext * context)
  {
    if (!action_search_answers_for_question.IsValid())
    {
      action_search_answers_for_question = context->SearchElementBySystemIdentifier("action_search_answers_for_question");
      nrel_answer = context->SearchElementBySystemIdentifier("nrel_answer");
    }
  }
};

}  // namespace utils

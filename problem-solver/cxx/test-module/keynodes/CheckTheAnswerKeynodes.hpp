#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_memory.hpp>

namespace utils
{

class CheckTheAnswerKeynodes : public ScObject
{
public:
  static inline ScAddr action_check_answer;
  static inline ScAddr nrel_selected_answers;
  static inline ScAddr nrel_answer;
  static inline ScAddr concept_correct_answer;
  static inline ScAddr concept_correct_user_answer;
  static inline ScAddr concept_incorrect_user_answer;

  static void Initialize(ScMemoryContext * context)
  {
    if (!action_check_answer.IsValid())
    {
      action_check_answer = context->SearchElementBySystemIdentifier("action_check_answer");
      nrel_selected_answers = context->SearchElementBySystemIdentifier("nrel_selected_answers");
      nrel_answer = context->SearchElementBySystemIdentifier("nrel_answer");
      concept_correct_answer = context->SearchElementBySystemIdentifier("concept_correct_answer");
      concept_correct_user_answer = context->SearchElementBySystemIdentifier("concept_correct_user_answer");
      concept_incorrect_user_answer = context->SearchElementBySystemIdentifier("concept_incorrect_user_answer");
    }
  }
};

}  // namespace utils

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_memory.hpp>

namespace utils
{

class ChoiceNextQuestionKeynodes : public ScObject
{
public:
  static inline ScAddr action_choice_next_question;
  static inline ScAddr concept_question;
  static inline ScAddr nrel_current_question;
  static inline ScAddr nrel_asked_questions;  // <- ДОБАВЬ ЭТУ СТРОКУ!

  static void Initialize(ScMemoryContext * context)
  {
    if (!action_choice_next_question.IsValid())
    {
      action_choice_next_question = context->SearchElementBySystemIdentifier("action_choice_next_question");
      concept_question = context->SearchElementBySystemIdentifier("concept_question");
      nrel_current_question = context->SearchElementBySystemIdentifier("nrel_current_question");
      nrel_asked_questions = context->SearchElementBySystemIdentifier("nrel_asked_questions");  // <- И ЭТУ!
    }
  }
};

}  // namespace utils

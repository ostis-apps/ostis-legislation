#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_memory.hpp>

namespace utils
{

class SaveAnswerKeynodes : public ScObject
{
public:
  static inline ScAddr action_save_answer;
  static inline ScAddr nrel_selected_answers;
  static inline ScAddr concept_answer;

  static void Initialize(ScMemoryContext * context)
  {
    if (!action_save_answer.IsValid())
    {
      action_save_answer = context->SearchElementBySystemIdentifier("action_save_answer");
      nrel_selected_answers = context->SearchElementBySystemIdentifier("nrel_selected_answers");
      concept_answer = context->SearchElementBySystemIdentifier("concept_answer");
    }
  }
};

}  // namespace utils

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_memory.hpp>

namespace utils
{

class RatingUpdateKeynodes : public ScObject
{
public:
  static inline ScAddr action_update_rating;
  static inline ScAddr nrel_selected_answers;
  static inline ScAddr concept_correct_answer;
  static inline ScAddr concept_incorrect_answer;

  static void Initialize(ScMemoryContext * context)
  {
    if (!action_update_rating.IsValid())
    {
      action_update_rating = context->SearchElementBySystemIdentifier("action_update_rating");
      nrel_selected_answers = context->SearchElementBySystemIdentifier("nrel_selected_answers");
      concept_correct_answer = context->SearchElementBySystemIdentifier("concept_correct_answer");
      concept_incorrect_answer = context->SearchElementBySystemIdentifier("concept_incorrect_answer");
    }
  }
};

}  // namespace utils

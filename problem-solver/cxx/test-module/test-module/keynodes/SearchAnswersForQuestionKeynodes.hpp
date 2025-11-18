#pragma once

#include <sc-memory/sc_keynodes.hpp>

class SearchAnswersForQuestionKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_search_answers_for_question{
      "action_search_answers_for_question",
      ScType::ConstNodeClass};

  static inline ScKeynode const nrel_answer{"nrel_answer", ScType::ConstNodeNonRole};
};

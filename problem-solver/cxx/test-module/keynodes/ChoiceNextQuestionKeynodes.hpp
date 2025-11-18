#pragma once

#include <sc-memory/sc_keynodes.hpp>

class ChoiceNextQuestionKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_choice_next_question{"action_choice_next_question", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_asked_questions{"nrel_asked_questions", ScType::ConstNodeNonRole};
};

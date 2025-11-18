#pragma once

#include <sc-memory/sc_keynodes.hpp>

class SaveAnswerKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const nrel_user_answer{"nrel_user_answer", ScType::ConstNodeNonRole};
  static inline ScKeynode const concept_correct_answer{"concept_correct_answer", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_selected_answers{"nrel_selected_answers", ScType::ConstNodeNonRole};
};

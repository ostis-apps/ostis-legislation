#pragma once

#include <sc-memory/sc_keynodes.hpp>

class RatingUpdateKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_rating_update{"action_rating_update", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_tier{"nrel_tier", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_theme_tier{"nrel_theme_tier", ScType::ConstNodeNonRole};

  static inline ScKeynode const concept_first_tier{"concept_first_tier", ScType::ConstNodeClass};
  static inline ScKeynode const concept_second_tier{"concept_second_tier", ScType::ConstNodeClass};
  static inline ScKeynode const concept_third_tier{"concept_third_tier", ScType::ConstNodeClass};
};

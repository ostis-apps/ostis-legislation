#pragma once

#include <sc-memory/sc_keynodes.hpp>

class DelEventKeynodes : public ScKeynodes {
public:
  static inline ScKeynode const action_del_event{"action_del_event", ScType::NodeConstClass};
  static inline ScKeynode const nrel_user_event{"nrel_user_event", ScType::NodeConstNoRole};
  static inline ScKeynode const nrel_event_name{"nrel_event_name", ScType::NodeConstNoRole};
  static inline ScKeynode const registered_user{"registered_jurisprudence_user", ScType::NodeConstClass};
};

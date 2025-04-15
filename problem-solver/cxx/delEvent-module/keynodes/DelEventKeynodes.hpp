#pragma once

#include <sc-memory/sc_keynodes.hpp>

class DelEventKeynodes : public ScKeynodes {
public:

  static inline ScKeynode const NREL_EVENT_NAME{"nrel_event_name", ScType::ConstNodeNonRole};

  static inline ScKeynode const action_del_event{"action_del_event", ScType::NodeConstClass};

};

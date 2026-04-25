#pragma once
#include <sc-memory/sc_memory.hpp>

namespace rateMessageModule
{

class Keynodes
{
public:
  static inline ScKeynode const action_rate_message{"action_rate_message", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_likes{"nrel_likes", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_dislikes{"nrel_dislikes", ScType::ConstNodeNonRole};

  static inline ScKeynode const rrel_1{"rrel_1", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_2{"rrel_2", ScType::ConstNodeRole};
};

} // namespace rateMessageModule

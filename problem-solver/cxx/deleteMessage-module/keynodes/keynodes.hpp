#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace deleteMessageModule
{

class Keynodes
{
public:
  static inline ScKeynode const action_delete_message{"action_delete_message", ScType::ConstNodeClass};
  static inline ScKeynode const action_initiated{"action_initiated", ScType::ConstNodeClass};

  static inline ScKeynode const concept_message{"concept_message", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_message_content{"nrel_message_content", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_message_author{"nrel_author", ScType::ConstNodeNonRole};

  static inline ScKeynode const rrel_1{"rrel_1", ScType::ConstNodeRole};
};

} // namespace deleteMessageModule

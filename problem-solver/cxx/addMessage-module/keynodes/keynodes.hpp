#pragma once

#include <sc-memory/sc_memory.hpp>

namespace addMessageModule
{

class Keynodes
{
public:
  static inline ScKeynode const action_add_message{"action_add_message", ScType::ConstNodeClass};
  static inline ScKeynode const action_initiated{"action_initiated", ScType::ConstNodeClass};
  
  static inline ScKeynode const concept_message{"concept_message", ScType::ConstNodeClass};
  static inline ScKeynode const concept_topic{"concept_topic", ScType::ConstNodeClass};
  
  static inline ScKeynode const nrel_message_content{"nrel_message_content", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_message_author{"nrel_author", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_topic_message{"nrel_topic_message", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const registered_user{"registered_jurisprudence_user", ScType::ConstNodeClass};
  
  static inline ScKeynode const rrel_1{"rrel_1", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_2{"rrel_2", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_3{"rrel_3", ScType::ConstNodeRole};
};

} // namespace addMessageModule

#pragma once

#include <sc-memory/sc_memory.hpp>

namespace addTopicModule
{

class AddTopicKeynodes
{
public:
  static inline ScKeynode const action_add_topic{"action_add_topic", ScType::ConstNodeClass};
  static inline ScKeynode const action_initiated{"action_initiated", ScType::ConstNodeClass};
  
  static inline ScKeynode const TOPIC_CLASS{"concept_topic", ScType::ConstNodeClass};
  
  static inline ScKeynode const NREL_TOPIC_TITLE{"nrel_topic_title", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_TOPIC_DESCRIPTION{"nrel_topic_description", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_TOPIC_AUTHOR{"nrel_author", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const REGISTERED_USER{"registered_jurisprudence_user", ScType::ConstNodeClass};
  
  static inline ScKeynode const rrel_1{"rrel_1", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_2{"rrel_2", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_3{"rrel_3", ScType::ConstNodeRole};
};

} // namespace addTopicModule

#pragma once

#include <sc-memory/sc_keynodes.hpp>

class DelEventKeynodes : public ScKeynodes {
public:
  // Узел действия агента удаления
  static inline ScKeynode const action_del_event{"action_del_event", ScType::NodeConstClass};

  // Данные для ввода пользователем
  static inline ScKeynode const NREL_EVENT_NAME{"nrel_event_name", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_EVENT{"nrel_user_event", ScType::NodeConstNoRole};
  static inline ScKeynode const REGISTERED_USER{"registered_jurisprudence_user", ScType::NodeConstClass};
};

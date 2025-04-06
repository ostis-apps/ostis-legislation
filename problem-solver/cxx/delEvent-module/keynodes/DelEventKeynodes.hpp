#pragma once

#include <sc-memory/sc_keynodes.hpp>

class DelEventKeynodes : public ScKeynodes {
public:
  // Данные для ввода пользователем
  //static inline ScKeynode const ADD_EVENT{"add_event", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_EVENT_NAME{"nrel_event_name", ScType::ConstNodeNonRole};
  // static inline ScKeynode const NREL_EVENT_DATE{"nrel_event_date", ScType::ConstNodeNonRole};
  // static inline ScKeynode const NREL_EVENT_DESCRIPTION{"nrel_event_description", ScType::ConstNodeNonRole};


  // Составляющие определенного дня
  // static inline ScKeynode const RREL_EVENT_DAY{"rrel_event_day", ScType::ConstNodeRole};
  // static inline ScKeynode const RREL_EVENT_MONTH{"rrel_event_month", ScType::ConstNodeRole};
  // static inline ScKeynode const RREL_EVENT_YEAR{"rrel_event_year", ScType::ConstNodeRole};

  // Узел действия агента добавления
  static inline ScKeynode const action_del_event{"action_del_event", ScType::NodeConstClass};

};

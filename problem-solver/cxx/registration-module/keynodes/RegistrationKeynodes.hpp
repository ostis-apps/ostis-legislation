#pragma once

#include <sc-memory/sc_keynodes.hpp>

class RegistrationKeynodes : public ScKeynodes {
public:
  // Данные для ввода пользователем
  static inline ScKeynode const REGISTERED_JURISPRUDENCE_USER{"registered_jurisprudence_user", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_PASSWORD{"nrel_user_password", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_LOGIN{"nrel_user_login", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_NAME{"nrel_user_name", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_SURNAME{"nrel_user_surname", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_PATRONYMIC{"nrel_user_patronymic", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_BIRTHDATE{"nrel_user_birthdate", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_ADDRESS{"nrel_user_address", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_USER_GENDER{"nrel_user_gender", ScType::ConstNodeNonRole};

  // Составляющие определенного дня
  static inline ScKeynode const RREL_USER_DAY{"rrel_user_day", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_USER_MONTH{"rrel_user_month", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_USER_YEAR{"rrel_user_year", ScType::ConstNodeRole};

  // Узел действия агента
  static inline ScKeynode const action_register{"action_register", ScType::NodeConstClass};
};
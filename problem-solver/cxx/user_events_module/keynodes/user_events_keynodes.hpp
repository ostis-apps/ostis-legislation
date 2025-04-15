#include <sc-memory/sc_keynodes.hpp>

class UserEventsKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_user_events{
    "action_user_events", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_user_event{
    "nrel_user_event", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_displayed_events{
    "nrel_displayed_events", ScType::ConstNodeNonRole};
  static inline ScKeynode const concept_user{
    "concept_user", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_main_idtf{
    "nrel_main_idtf", ScType::ConstNodeNonRole};
  static inline ScKeynode const rrel_key_sc_element{
    "rrel_key_sc_element", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_example{
    "rrel_example", ScType::ConstNodeRole};
  static inline ScKeynode const nrel_sc_text_translation{
    "nrel_sc_text_translation", ScType::ConstNodeNonRole};
  static inline ScKeynode const lang_ru{
    "lang_ru", ScType::ConstNodeClass};
  static inline ScKeynode const lang_by{
    "lang_by", ScType::ConstNodeClass};
  static inline ScKeynode const lang_en{
    "lang_en", ScType::ConstNodeClass};

  static inline ScKeynode const NREL_EVENT_NAME{"nrel_event_name", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_EVENT_DATE{"nrel_event_date", ScType::ConstNodeNonRole};
  static inline ScKeynode const NREL_EVENT_DESCRIPTION{"nrel_event_description", ScType::ConstNodeNonRole};

  static inline ScKeynode const RREL_EVENT_DAY{"rrel_event_day", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_EVENT_MONTH{"rrel_event_month", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_EVENT_YEAR{"rrel_event_year", ScType::ConstNodeRole};

};
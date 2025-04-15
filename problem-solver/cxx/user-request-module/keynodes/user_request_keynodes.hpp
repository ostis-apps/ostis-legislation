#include <sc-memory/sc_keynodes.hpp>

// This class unites keynodes that used by agents of one module 
// (with one sense). You can implement hierarchies of keynodes.
class UserRequestKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_user_request{
    "action_user_request", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_user_request{
    "nrel_user_request", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_main_idtf{
    "nrel_main_idtf", ScType::ConstNodeNonRole};
  static inline ScKeynode const rrel_key_sc_element{
    "rrel_key_sc_element", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_example{
    "rrel_example", ScType::ConstNodeRole};
  static inline ScKeynode const nrel_sc_text_translation{
    "nrel_sc_text_translation", ScType::ConstNodeNonRole};
  static inline ScKeynode const belarus_legal_term{
    "belarus_legal_term", ScType::ConstNodeClass};
  static inline ScKeynode const lang_ru{
    "lang_ru", ScType::ConstNodeClass};
  static inline ScKeynode const lang_by{
    "lang_by", ScType::ConstNodeClass};
  static inline ScKeynode const lang_en{
    "lang_en", ScType::ConstNodeClass};
  
  // Here the first argument in constructor is system identifier of 
  // sc-keynode and the second argument is sc-type of this sc-keynode.
  // If there is no sc-keynode with such system identifier in 
  // knowledge base, then the one with specified sc-type will be generated.
  // You don't should specify type of sc-keynode here, be default it is 
  // `ScType::ConstNode`. But you sure that your code will use this 
  // keynode with type `ScType::ConstNode` correctly.
};
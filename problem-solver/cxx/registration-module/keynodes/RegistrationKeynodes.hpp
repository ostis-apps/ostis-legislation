#pragma once

#include <sc-memory/sc_keynodes.hpp>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <string>

class RegistrationKeynodes : public ScKeynodes {
public:
  // Action class for registration
  static inline ScKeynode const action_user_registration{
    "action_user_registration", ScType::ConstNodeClass};
  
  // User types
  static inline ScKeynode const concept_client{
    "concept_client", ScType::ConstNodeClass};
  
  static inline ScKeynode const concept_specialist{
    "concept_specialist", ScType::ConstNodeClass};
  
  static inline ScKeynode const concept_user{
    "concept_user", ScType::ConstNodeClass};
  
  // User attributes relations
  static inline ScKeynode const nrel_email{
    "nrel_email", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_password{
    "nrel_password", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_system_identifier{
    "nrel_system_identifier", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_password_confirmation{
    "nrel_password_confirmation", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_user_type{
    "nrel_user_type", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_full_name{
    "nrel_full_name", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_gender{
    "nrel_gender", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_age{
    "nrel_age", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_experience{
    "nrel_experience", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_field{
    "nrel_field", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_verification_token{
    "nrel_verification_token", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_user_id{
    "nrel_user_id", ScType::ConstNodeNonRole};
  
  // Registration status
  static inline ScKeynode const concept_registration_success{
    "concept_registration_success", ScType::ConstNodeClass};
  
  static inline ScKeynode const concept_registration_error{
    "concept_registration_error", ScType::ConstNodeClass};
  
  static inline ScKeynode const nrel_error_message{
    "nrel_error_message", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_status{
    "nrel_status", ScType::ConstNodeNonRole};
  
  static inline ScKeynode const nrel_result{
    "nrel_result", ScType::ConstNodeNonRole};
  
  // Role relations for action arguments
  static inline ScKeynode const rrel_1{
    "rrel_1", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_2{
    "rrel_2", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_3{
    "rrel_3", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_4{
    "rrel_4", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_5{
    "rrel_5", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_6{
    "rrel_6", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_7{
    "rrel_7", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_8{
    "rrel_8", ScType::ConstNodeRole};
  
  static inline ScKeynode const rrel_9{
    "rrel_9", ScType::ConstNodeRole};
  
  // Main identifier relation
  static inline ScKeynode const nrel_main_idtf{
    "nrel_main_idtf", ScType::ConstNodeNonRole};
  
  // Languages
  static inline ScKeynode const lang_ru{
    "lang_ru", ScType::ConstNodeClass};
  
  static inline ScKeynode const lang_en{
    "lang_en", ScType::ConstNodeClass};

  // Составляющие определенного дня
  static inline ScKeynode const RREL_USER_DAY{"rrel_user_day", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_USER_MONTH{"rrel_user_month", ScType::ConstNodeRole};
  static inline ScKeynode const RREL_USER_YEAR{"rrel_user_year", ScType::ConstNodeRole};

  static inline ScKeynode const action_authentication{"action_authentication", ScType::ConstNodeClass};
  static inline ScKeynode const action_verification{"action_verification", ScType::ConstNodeClass};
  static inline ScKeynode const action_initiated{"action_initiated", ScType::ConstNodeClass};

  // Для верификации email
  static inline ScKeynode const concept_verified_user{
    "concept_verified_user", ScType::ConstNodeClass};

  // Функция хеширования пароля
  static std::string HashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
  }
};
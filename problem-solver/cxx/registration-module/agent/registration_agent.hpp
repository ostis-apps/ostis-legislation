#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/RegistrationKeynodes.hpp"

class ScRegistrationAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  // Validation methods
  bool validate_email(std::string const & email);
  bool validate_password(std::string const & password);
  bool check_password_match(std::string const & password, std::string const & confirmation);
  bool check_email_exists(std::string const & email);

  // User creation methods
  ScAddr create_user_node();
  void set_user_attribute(ScAddr const & user_node, ScAddr const & relation, std::string const & value);
  void set_user_attribute_link(ScAddr const & user_node, ScAddr const & relation, ScAddr const & link_node);
  std::string generate_verification_token(std::string const & email);
  std::string generate_user_id();

  // Client registration
  ScAddr register_client(
    std::string const & email,
    std::string const & password,
    ScStructure & result);

  // Specialist registration
  ScAddr register_specialist(
    std::string const & email,
    std::string const & password,
    std::string const & full_name,
    std::string const & gender,
    std::string const & age,
    std::string const & experience,
    std::string const & field,
    ScStructure & result);

  // Result creation
  void create_success_result(
  ScAddr const & user_node,
  std::string const & user_id,
  ScStructure & result);


  void create_error_result(
    std::string const & error_message,
    ScStructure & result);

  // Email verification
  bool send_verification_token(std::string const & email);
};

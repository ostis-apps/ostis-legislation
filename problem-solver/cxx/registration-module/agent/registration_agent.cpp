#include "registration_agent.hpp"

#include <regex>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "keynodes/RegistrationKeynodes.hpp"

#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace utils;

// Helper function to read string from ScStream
std::string readStringFromLink(ScMemoryContext & context, ScAddr const & linkAddr)
{
  ScStreamPtr stream = context.GetLinkContent(linkAddr);
  if (!stream || stream->Size() == 0)
    return "";

  std::string result;
  result.resize(stream->Size());
  size_t readBytes = 0;
  stream->Read(&result[0], stream->Size(), readBytes);
  return result;
}

ScAddr ScRegistrationAgent::GetActionClass() const
{
  return RegistrationKeynodes::action_user_registration;
}

ScResult ScRegistrationAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("Registration agent started");

  ScStructure result = m_context.GenerateStructure();

  try
  {
    // Get action arguments
    // rrel_1: email, rrel_2: password, rrel_3: password_confirmation, rrel_4: user_type
    // For specialist: rrel_5: name, rrel_6: gender, rrel_7: age, rrel_8: experience, rrel_9: field

    ScAddr email_node, password_node, password_conf_node, user_type_node;

    ScTemplate arg_template;
    ScTemplateSearchResult search_result;

    // Get email (rrel_1)
    arg_template.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNodeLink >> "_email"
    );
    arg_template.Triple(
      RegistrationKeynodes::rrel_1,
      ScType::VarPermPosArc,
      "_arc"
    );

    if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
    {
      SC_LOG_ERROR("Email argument not found");
      create_error_result("Email is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    email_node = search_result[0]["_email"];

    // Get email value
    std::string email = readStringFromLink(m_context, email_node);

    // Validate email
    if (!validate_email(email))
    {
      SC_LOG_ERROR("Invalid email format");
      create_error_result("Invalid email format", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    // Check if email already exists
    if (check_email_exists(email))
    {
      SC_LOG_ERROR("Email already registered");
      create_error_result("Email already registered", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    // Get password (rrel_2)
    arg_template.Clear();
    arg_template.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNodeLink >> "_password"
    );
    arg_template.Triple(
      RegistrationKeynodes::rrel_2,
      ScType::VarPermPosArc,
      "_arc"
    );

    if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
    {
      SC_LOG_ERROR("Password argument not found");
      create_error_result("Password is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    password_node = search_result[0]["_password"];
    std::string password = readStringFromLink(m_context, password_node);

    // Validate password
    if (!validate_password(password))
    {
      SC_LOG_ERROR("Invalid password");
      create_error_result("Password must be at least 8 characters long and contain uppercase, lowercase, digit and special character", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    // Get password confirmation (rrel_3)
    arg_template.Clear();
    arg_template.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNodeLink >> "_password_conf"
    );
    arg_template.Triple(
      RegistrationKeynodes::rrel_3,
      ScType::VarPermPosArc,
      "_arc"
    );

    if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
    {
      SC_LOG_ERROR("Password confirmation argument not found");
      create_error_result("Password confirmation is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    password_conf_node = search_result[0]["_password_conf"];
    std::string password_conf = readStringFromLink(m_context, password_conf_node);

    // Check password match
    if (!check_password_match(password, password_conf))
    {
      SC_LOG_ERROR("Passwords do not match");
      create_error_result("Passwords do not match", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    // Get user type (rrel_4)
    arg_template.Clear();
    arg_template.Triple(
      action,
      ScType::VarPermPosArc >> "_arc",
      ScType::VarNode >> "_user_type"
    );
    arg_template.Triple(
      RegistrationKeynodes::rrel_4,
      ScType::VarPermPosArc,
      "_arc"
    );

    if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
    {
      SC_LOG_ERROR("User type argument not found");
      create_error_result("User type is required", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    user_type_node = search_result[0]["_user_type"];

    // Check user type and register accordingly
    ScAddr user_node;

    if (m_context.CheckConnector(RegistrationKeynodes::concept_client, user_type_node, ScType::ConstPermPosArc))
    {
      // Register as client
      SC_LOG_INFO("Registering client");
      user_node = register_client(email, password, result);
    }
    else if (m_context.CheckConnector(RegistrationKeynodes::concept_specialist, user_type_node, ScType::ConstPermPosArc))
    {
      // Register as specialist - get additional fields
      SC_LOG_INFO("Registering specialist");

      // Get full name (rrel_5)
      arg_template.Clear();
      arg_template.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_name"
      );
      arg_template.Triple(
        RegistrationKeynodes::rrel_5,
        ScType::VarPermPosArc,
        "_arc"
      );

      if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
      {
        SC_LOG_ERROR("Full name argument not found");
        create_error_result("Full name is required for specialist registration", result);
        action.SetResult(result);
        return action.FinishUnsuccessfully();
      }

      std::string full_name = readStringFromLink(m_context, search_result[0]["_name"]);

      // Get gender (rrel_6)
      arg_template.Clear();
      arg_template.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_gender"
      );
      arg_template.Triple(
        RegistrationKeynodes::rrel_6,
        ScType::VarPermPosArc,
        "_arc"
      );

      if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
      {
        SC_LOG_ERROR("Gender argument not found");
        create_error_result("Gender is required for specialist registration", result);
        action.SetResult(result);
        return action.FinishUnsuccessfully();
      }

      std::string gender = readStringFromLink(m_context, search_result[0]["_gender"]);

      // Get age (rrel_7)
      arg_template.Clear();
      arg_template.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_age"
      );
      arg_template.Triple(
        RegistrationKeynodes::rrel_7,
        ScType::VarPermPosArc,
        "_arc"
      );

      if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
      {
        SC_LOG_ERROR("Age argument not found");
        create_error_result("Age is required for specialist registration", result);
        action.SetResult(result);
        return action.FinishUnsuccessfully();
      }

      std::string age = readStringFromLink(m_context, search_result[0]["_age"]);

      // Get experience (rrel_8)
      arg_template.Clear();
      arg_template.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_experience"
      );
      arg_template.Triple(
        RegistrationKeynodes::rrel_8,
        ScType::VarPermPosArc,
        "_arc"
      );

      if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
      {
        SC_LOG_ERROR("Experience argument not found");
        create_error_result("Experience is required for specialist registration", result);
        action.SetResult(result);
        return action.FinishUnsuccessfully();
      }

      std::string experience = readStringFromLink(m_context, search_result[0]["_experience"]);

      // Get field (rrel_9)
      arg_template.Clear();
      arg_template.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_field"
      );
      arg_template.Triple(
        RegistrationKeynodes::rrel_9,
        ScType::VarPermPosArc,
        "_arc"
      );

      if (!m_context.SearchByTemplate(arg_template, search_result) || search_result.IsEmpty())
      {
        SC_LOG_ERROR("Field argument not found");
        create_error_result("Field is required for specialist registration", result);
        action.SetResult(result);
        return action.FinishUnsuccessfully();
      }

      std::string field = readStringFromLink(m_context, search_result[0]["_field"]);

      user_node = register_specialist(email, password, full_name, gender, age, experience, field, result);
    }
    else
    {
      SC_LOG_ERROR("Invalid user type");
      create_error_result("Invalid user type", result);
      action.SetResult(result);
      return action.FinishUnsuccessfully();
    }

    if (!m_context.IsElement(user_node))
    {
      SC_LOG_ERROR("Failed to create user");
      create_error_result("Failed to create user", result);
      action.SetResult(result);
      return action.FinishWithError();
    }

    // После успешной регистрации запускаем отправку токена верификации
    SC_LOG_INFO("User registered successfully, sending verification token");

    bool tokenSent = send_verification_token(email);
    if (!tokenSent)
    {
      SC_LOG_ERROR("Failed to send verification token");
      create_error_result("Registration successful but failed to send verification email. Please contact support.", result);
      action.SetResult(result);
      return action.FinishWithError();
    }

    SC_LOG_INFO("User registered successfully and verification token sent");

    // Успешный результат (без собственного токена: VerificationAgent сам всё делает)
    create_success_result(user_node, generate_user_id(), result);
    action.SetResult(result);
    return action.FinishSuccessfully();
  }
  catch (ScException const & exception)
  {
    SC_LOG_ERROR(exception.Message());
    create_error_result("Internal error during registration", result);
    action.SetResult(result);
    return action.FinishWithError();
  }
}

bool ScRegistrationAgent::validate_email(std::string const & email)
{
  std::regex email_regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
  return std::regex_match(email, email_regex);
}

bool ScRegistrationAgent::validate_password(std::string const & password)
{
  // Check minimum length
  if (password.length() < 8)
    return false;
  
  // Check for uppercase letter
  if (!std::regex_search(password, std::regex("[A-Z]")))
    return false;
  
  // Check for lowercase letter
  if (!std::regex_search(password, std::regex("[a-z]")))
    return false;
  
  // Check for digit
  if (!std::regex_search(password, std::regex("[0-9]")))
    return false;
  
  // Check for special character
  if (!std::regex_search(password, std::regex("[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]")))
    return false;
  
  return true;
}

bool ScRegistrationAgent::check_password_match(std::string const & password, std::string const & confirmation)
{
  return password == confirmation;
}

bool ScRegistrationAgent::check_email_exists(std::string const & email)
{
  ScTemplate email_template;
  email_template.Quintuple(
    ScType::VarNode >> "_user",
    ScType::VarCommonArc,
    ScType::VarNodeLink >> "_link",
    ScType::VarPermPosArc,
    RegistrationKeynodes::nrel_email
  );

  ScTemplateSearchResult search_result;
  m_context.SearchByTemplate(email_template, search_result);

  for (size_t i = 0; i < search_result.Size(); i++)
  {
    ScAddr link_node = search_result[i]["_link"];
    std::string existing_email = readStringFromLink(m_context, link_node);
    if (existing_email == email)
      return true;
  }

  return false;
}

ScAddr ScRegistrationAgent::create_user_node()
{
  return m_context.GenerateNode(ScType::ConstNode);
}

void ScRegistrationAgent::set_user_attribute(
  ScAddr const & user_node,
  ScAddr const & relation,
  std::string const & value)
{
  ScAddr link_node = m_context.GenerateLink();
  m_context.SetLinkContent(link_node, value);
  ScAddr arc = m_context.GenerateConnector(ScType::ConstCommonArc, user_node, link_node);
  m_context.GenerateConnector(ScType::ConstPermPosArc, relation, arc);
}

void ScRegistrationAgent::set_user_attribute_link(
  ScAddr const & user_node,
  ScAddr const & relation,
  ScAddr const & link_node)
{
  ScAddr arc = m_context.GenerateConnector(ScType::ConstCommonArc, user_node, link_node);
  m_context.GenerateConnector(ScType::ConstPermPosArc, relation, arc);
}

// generate_verification_token БОЛЬШЕ НЕ НУЖЕН для БЗ, но может использоваться только в UI-результате при желании.
// Можно оставить или удалить. Оставим, но он больше нигде не вызывается.
std::string ScRegistrationAgent::generate_verification_token(std::string const & email)
{
  auto now = std::chrono::system_clock::now();
  auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

  std::stringstream ss;
  ss << std::hex << std::setfill('0');

  for (size_t i = 0; i < email.length(); i++)
    ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(email[i]));

  ss << std::setw(16) << timestamp;
  return ss.str();
}

std::string ScRegistrationAgent::generate_user_id()
{
  auto now = std::chrono::system_clock::now();
  auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1000, 9999);

  std::stringstream ss;
  ss << "user_" << timestamp << "_" << dis(gen);
  return ss.str();
}

bool ScRegistrationAgent::send_verification_token(std::string const & email)
{
  try
  {
    std::cout << "=== Отправка токена верификации ===" << std::endl;
    std::cout << "Email: " << email << std::endl;

    // Создаем действие верификации
    ScAddr action_node = m_context.CreateNode(ScType::NodeConst);
    m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, RegistrationKeynodes::action_verification, action_node);

    // Добавляем email как аргумент (rrel_1)
    ScAddr email_link = m_context.CreateLink(ScType::LinkConst);
    m_context.SetLinkContent(email_link, email);
    ScAddr email_arc = m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, action_node, email_link);
    m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, RegistrationKeynodes::rrel_1, email_arc);

    // Инициируем действие
    m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, RegistrationKeynodes::action_initiated, action_node);

    std::cout << "Действие верификации создано и инициировано для: " << email << std::endl;
    std::cout << "Адрес действия: " << action_node.Hash() << std::endl;

    return true;
  }
  catch (const std::exception & e)
  {
    std::cout << "Ошибка отправки токена верификации: " << e.what() << std::endl;
    return false;
  }
}

ScAddr ScRegistrationAgent::register_client(
    std::string const & email,
    std::string const & password,
    ScStructure & result)
{
    std::string hashedPassword = RegistrationKeynodes::HashPassword(password);

    // Create user node
    ScAddr usernode = create_user_node();

    // Add to concept_user and concept_client
    ScAddr userarc = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::concept_user, usernode);
    ScAddr clientarc = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::concept_client, usernode);

    // Set attributes - ИЗМЕНЕНО: используем nrel_system_identifier вместо nrel_email
    set_user_attribute(usernode, RegistrationKeynodes::nrel_system_identifier, email);
    set_user_attribute(usernode, RegistrationKeynodes::nrel_password, hashedPassword);

    // Generate user ID
    std::string userid = generate_user_id();
    set_user_attribute(usernode, RegistrationKeynodes::nrel_user_id, userid);

    // Add to result structure
    result << usernode << userarc << clientarc;

    return usernode;
}


ScAddr ScRegistrationAgent::register_specialist(
    std::string const & email,
    std::string const & password,
    std::string const & fullname,
    std::string const & gender,
    std::string const & age,
    std::string const & experience,
    std::string const & field,
    ScStructure & result)
{
    std::string hashedPassword = RegistrationKeynodes::HashPassword(password);

    // Create user node
    ScAddr usernode = create_user_node();

    // Add to concept_user and concept_specialist
    ScAddr userarc = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::concept_user, usernode);
    ScAddr specialistarc = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::concept_specialist, usernode);

    // Basic attributes - ИЗМЕНЕНО: используем nrel_system_identifier вместо nrel_email
    set_user_attribute(usernode, RegistrationKeynodes::nrel_system_identifier, email);
    set_user_attribute(usernode, RegistrationKeynodes::nrel_password, hashedPassword);

    // Specialist-specific attributes
    set_user_attribute(usernode, RegistrationKeynodes::nrel_full_name, fullname);
    set_user_attribute(usernode, RegistrationKeynodes::nrel_gender, gender);
    set_user_attribute(usernode, RegistrationKeynodes::nrel_age, age);
    set_user_attribute(usernode, RegistrationKeynodes::nrel_experience, experience);
    set_user_attribute(usernode, RegistrationKeynodes::nrel_field, field);

    // Generate user ID
    std::string userid = generate_user_id();
    set_user_attribute(usernode, RegistrationKeynodes::nrel_user_id, userid);

    result << usernode << userarc << specialistarc;

    return usernode;
}

void ScRegistrationAgent::create_success_result(
  ScAddr const & user_node,
  std::string const & user_id,
  ScStructure & result)
{
  ScAddr status_node = m_context.GenerateNode(ScType::ConstNode);
  ScAddr status_arc = m_context.GenerateConnector(
    ScType::ConstPermPosArc,
    RegistrationKeynodes::concept_registration_success,
    status_node
  );

  ScAddr status_link = m_context.GenerateLink();
  m_context.SetLinkContent(status_link, "success");
  ScAddr status_conn = m_context.GenerateConnector(ScType::ConstCommonArc, status_node, status_link);
  m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_status, status_conn);

  result << status_node << status_arc << status_link << status_conn;

  SC_LOG_INFO("Registration successful for user: " + user_id);
}


void ScRegistrationAgent::create_error_result(
  std::string const & error_message,
  ScStructure & result)
{
  ScAddr error_node = m_context.GenerateNode(ScType::ConstNode);
  ScAddr error_arc = m_context.GenerateConnector(
    ScType::ConstPermPosArc,
    RegistrationKeynodes::concept_registration_error,
    error_node
  );

  ScAddr error_link = m_context.GenerateLink();
  m_context.SetLinkContent(error_link, error_message);
  ScAddr error_conn = m_context.GenerateConnector(ScType::ConstCommonArc, error_node, error_link);
  m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_error_message, error_conn);

  ScAddr status_link = m_context.GenerateLink();
  m_context.SetLinkContent(status_link, "error");
  ScAddr status_conn = m_context.GenerateConnector(ScType::ConstCommonArc, error_node, status_link);
  m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_status, status_conn);

  result << error_node << error_arc << error_link << error_conn << status_link << status_conn;

  SC_LOG_ERROR("Registration error: " + error_message);
}

#include "AuthenticationAgent.hpp"
#include "keynodes/RegistrationKeynodes.hpp"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <string>

std::string HashPassword(const std::string password) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, password.c_str(), password.length());
  SHA256_Final(hash, &sha256);

  std::stringstream ss;
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return ss.str();
}

bool isUserWithLoginExist(ScAgentContext& context, ScAddr const & loginAddr, ScAddr& userAddr) {
  ScTemplate templateForUserLogin;

  templateForUserLogin.Triple(
      RegistrationKeynodes::REGISTERED_JURISPRUDENCE_USER,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_login"
  );

  ScTemplateSearchResult userTemplateSearchResult;
  context.SearchByTemplate(templateForUserLogin, userTemplateSearchResult);

  if (!userTemplateSearchResult.IsEmpty()) {
    for (size_t i = 0; i < userTemplateSearchResult.Size(); i++) {
      ScTemplateResultItem userTemplateResultItem;
      userTemplateSearchResult.Get(i, userTemplateResultItem);

      ScAddr login;
      userTemplateResultItem.Get("_login", login);

      std::string inputLogin;
      context.GetLinkContent(loginAddr, inputLogin);

      if (context.GetElementSystemIdentifier(login) == inputLogin) {
        userTemplateResultItem.Get("_login", userAddr);
        return true;
      }
    }
  }

  return false;
}

ScAddr AuthenticationAgent::GetActionClass() const {
  return RegistrationKeynodes::action_authentication;
}

ScResult AuthenticationAgent::DoProgram(ScAction & action) {
  auto const & [loginAddr, passwordAddr] = action.GetArguments<2>();

  std::string login;
  std::string password;

  m_context.GetLinkContent(loginAddr, login);
  m_context.GetLinkContent(passwordAddr, password);
  std::string hashedPassword = HashPassword(password);

  SC_LOG_INFO("Логин: " + login);
  SC_LOG_INFO("Хэшированный пароль: " + hashedPassword);

  ScAddr userAddr;
  if (isUserWithLoginExist(m_context, loginAddr, userAddr)) {
    SC_LOG_INFO("Пользователь с таким логином найден.");

    ScTemplate passwordTemplate;
    passwordTemplate.Quintuple(
        userAddr,
        ScType::CommonArc,
        ScType::VarNode >> "_password",
        ScType::VarPermPosArc,
        RegistrationKeynodes::NREL_USER_PASSWORD
    );

    ScTemplateSearchResult passwordTemplateSearchResult;
    m_context.SearchByTemplate(passwordTemplate, passwordTemplateSearchResult);

    if (passwordTemplateSearchResult.IsEmpty()) {
      SC_LOG_ERROR("Пароль пользователя не найден.");
      return action.FinishUnsuccessfully();
    }

    ScTemplateResultItem passwordTemplateResultItem;
    passwordTemplateSearchResult.Get(0, passwordTemplateResultItem);

    ScAddr passwordLinkAddr;
    passwordTemplateResultItem.Get("_password", passwordLinkAddr);

    std::string storedPassword;
    m_context.GetLinkContent(passwordLinkAddr, storedPassword);

    if (storedPassword == hashedPassword) {
      SC_LOG_INFO("Авторизация успешна!");
      return action.FinishSuccessfully();
    } else {
      SC_LOG_ERROR("Неверный пароль.");
      return action.FinishUnsuccessfully();
    }

  } else {
    SC_LOG_ERROR("Пользователь с таким логином не найден.");
    return action.FinishUnsuccessfully();
  }
}
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

bool isCorrectLoginAndPassword(ScAgentContext& context, const std::string& login, const std::string& password) {
  ScTemplate templateToCheckPassword;

  templateToCheckPassword.Triple(
      RegistrationKeynodes::REGISTERED_JURISPRUDENCE_USER,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_user"
  );

  templateToCheckPassword.Quintuple(
    "_user",
    ScType::VarCommonArc,
    ScType::VarNodeLink >> "_password",
    ScType::VarPermPosArc,
    RegistrationKeynodes::NREL_USER_PASSWORD
  );

  ScTemplateSearchResult templateToCheckPasswordSearchResult;
  context.SearchByTemplate(templateToCheckPassword, templateToCheckPasswordSearchResult);

  if (!templateToCheckPasswordSearchResult.IsEmpty()) {
    for (int i = 0; i < templateToCheckPasswordSearchResult.Size(); i++) {
      ScTemplateResultItem templateToCheckPasswordResultItem;
      templateToCheckPasswordSearchResult.Get(i, templateToCheckPasswordResultItem);

      ScAddr loginAddr;
      templateToCheckPasswordResultItem.Get("_user", loginAddr);

      ScAddr passwordAddr;
      std::string passwordInKnowledgeBase;
      templateToCheckPasswordResultItem.Get("_password", passwordAddr);
      context.GetLinkContent(passwordAddr, passwordInKnowledgeBase);

      SC_LOG_INFO("Пользователь " + std::to_string(i));
      SC_LOG_INFO("Логин в БЗ: " + context.GetElementSystemIdentifier(loginAddr));
      SC_LOG_INFO("Пароль в БЗ " + passwordInKnowledgeBase);

      if (context.GetElementSystemIdentifier(loginAddr) == login &&
        passwordInKnowledgeBase == password) {
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

  if (isCorrectLoginAndPassword(m_context, login, hashedPassword)) {
    SC_LOG_INFO("Автризация упешна!");
    return action.FinishSuccessfully();
  }
  SC_LOG_INFO("Авторизация безуспешна.");
  return action.FinishUnsuccessfully();
}
#include "RegistrationAgent.hpp"
#include "keynodes/RegistrationKeynodes.hpp"
#include <openssl/sha.h>
#include <iomanip>
std::string hashPassword(const std::string password) {
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

bool isUserLoginExist(ScAgentContext& context, const std::string& login) {
  ScTemplate templateForUsers;

  templateForUsers.Triple(
      RegistrationKeynodes::REGISTERED_JURISPRUDENCE_USER,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_login"
      );

  ScTemplateSearchResult templateForUsersSearchResult;
  context.SearchByTemplate(templateForUsers, templateForUsersSearchResult);

  if (!templateForUsersSearchResult.IsEmpty()) {
    ScTemplateResultItem templateForUsersTemplateResultItem;
    templateForUsersSearchResult.Get(0, templateForUsersTemplateResultItem);

    ScAddr loginAddr;

    templateForUsersTemplateResultItem.Get("_login", loginAddr);

    return context.GetElementSystemIdentifier(loginAddr) == login;
  }

  return false;
}

std::string getBirthDay(ScAgentContext& context, const ScAddr& birthdateTuple) {
  ScTemplate birthDayTemplate;

  birthDayTemplate.Quintuple(
      birthdateTuple,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_birth_day",
      ScType::VarPermPosArc,
      RegistrationKeynodes::RREL_USER_DAY
  );

  ScTemplateSearchResult birthDayTemplateSearchResult;
  context.SearchByTemplate(birthDayTemplate, birthDayTemplateSearchResult);

  if (!birthDayTemplateSearchResult.IsEmpty()) {
    ScTemplateResultItem birthDayTemplateResultItem;
    birthDayTemplateSearchResult.Get(0, birthDayTemplateResultItem);

    ScAddr birthDayAddr;

    birthDayTemplateResultItem.Get("_birth_day", birthDayAddr);

    return context.GetElementSystemIdentifier(birthDayAddr);
  }

  return "";
}

std::string getBirthMonth(ScAgentContext& context, const ScAddr& birthdateTuple) {
  ScTemplate birthMonthTemplate;

  birthMonthTemplate.Quintuple(
      birthdateTuple,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_birth_month",
      ScType::VarPermPosArc,
      RegistrationKeynodes::RREL_USER_MONTH
  );

  ScTemplateSearchResult birthMonthTemplateSearchResult;
  context.SearchByTemplate(birthMonthTemplate, birthMonthTemplateSearchResult);

  if (!birthMonthTemplateSearchResult.IsEmpty()) {
    ScTemplateResultItem birthMonthTemplateResultItem;
    birthMonthTemplateSearchResult.Get(0, birthMonthTemplateResultItem);

    ScAddr birthMonthAddr;

    birthMonthTemplateResultItem.Get("_birth_month", birthMonthAddr);

    return context.GetElementSystemIdentifier(birthMonthAddr);
  }

  return "";
}

std::string getBirthYear(ScAgentContext& context, const ScAddr& birthdateTuple) {
  ScTemplate birthYearTemplate;

  birthYearTemplate.Quintuple(
      birthdateTuple,
      ScType::VarPermPosArc,
      ScType::VarNode >> "_birth_year",
      ScType::VarPermPosArc,
      RegistrationKeynodes::RREL_USER_YEAR
  );

  ScTemplateSearchResult birthYearTemplateSearchResult;
  context.SearchByTemplate(birthYearTemplate, birthYearTemplateSearchResult);

  if (!birthYearTemplateSearchResult.IsEmpty()) {
    ScTemplateResultItem birthYearTemplateResultItem;
    birthYearTemplateSearchResult.Get(0, birthYearTemplateResultItem);

    ScAddr birthYearAddr;

    birthYearTemplateResultItem.Get("_birth_year", birthYearAddr);

    return context.GetElementSystemIdentifier(birthYearAddr);
  }

  return "";
}

ScAddr RegistrationAgent::GetActionClass() const
{
  return RegistrationKeynodes::action_register;
}

ScResult RegistrationAgent::DoProgram(ScAction & action)
{

  auto const & [loginAddr, passwordAddr, surnameAddr, nameAddr,
                patronymicAddr, birthdateAddr, addressAddr, genderAddr] = action.GetArguments<8>();

  // Данные для регистрации
  std::string login;
  std::string password;
  std::string surname;
  std::string name;
  std::string patronymic;
  std::string birthDay;
  std::string birthMonth;
  std::string birthYear;
  std::string address;
  std::string gender;

  // Получение данных в переменные
  m_context.GetLinkContent(loginAddr, login);
  m_context.GetLinkContent(passwordAddr, password);
  m_context.GetLinkContent(surnameAddr, surname);
  m_context.GetLinkContent(nameAddr, name);
  m_context.GetLinkContent(patronymicAddr, patronymic);
  m_context.GetLinkContent(addressAddr, address);
  gender = m_context.GetElementSystemIdentifier(genderAddr);
  birthDay = getBirthDay(m_context, birthdateAddr);
  birthMonth = getBirthMonth(m_context, birthdateAddr);
  birthYear = getBirthYear(m_context, birthdateAddr);

  SC_LOG_INFO("Логин: " + login);
  SC_LOG_INFO("Пароль: " + password);
  SC_LOG_INFO("Фамилия: " + surname);
  SC_LOG_INFO("Имя: " + name);
  SC_LOG_INFO("Отчество: " + patronymic);
  SC_LOG_INFO("День рождения: " + birthDay + "." + birthMonth + "." + birthYear);
  SC_LOG_INFO("Адрес: " + address);
  SC_LOG_INFO("Пол: " + gender);

  // Проверка на существование логина
  if (isUserLoginExist(m_context, login)) {
    SC_LOG_INFO("Данный пользователь уже существует!");
    return action.FinishUnsuccessfully();
  }

  // Генерация узлов для хранения пользователя
  ScAddr userAddr = m_context.GenerateNode(ScType::ConstNode);
  ScAddr userLoginAddr = m_context.GenerateLink();
  ScAddr userPasswordAddr = m_context.GenerateLink();
  ScAddr userSurnameAddr = m_context.GenerateLink();
  ScAddr userNameAddr = m_context.GenerateLink();
  ScAddr userPatronymicAddr = m_context.GenerateLink();
  ScAddr userBirthdayAddr = m_context.GenerateLink();
  ScAddr userAddressAddr = m_context.GenerateLink();

  // Заполнение узлов для хранения пользователя
  m_context.SetElementSystemIdentifier(login, userAddr);
  m_context.SetLinkContent(userLoginAddr, login);
  m_context.SetLinkContent(userPasswordAddr, hashPassword(password));
  m_context.SetLinkContent(userSurnameAddr, surname);
  m_context.SetLinkContent(userNameAddr, name);
  m_context.SetLinkContent(userPatronymicAddr, patronymic);
  m_context.SetLinkContent(userAddressAddr, address);
  m_context.SetLinkContent(userBirthdayAddr, birthDay + "." + birthMonth + "." + birthYear);

  // Ребра от узла пользователя к его параметрам
  ScAddr userRegisteredClassToUserNodeConnectorAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::REGISTERED_JURISPRUDENCE_USER, userAddr);
  ScAddr userNodeToUserLoginAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userLoginAddr);
  ScAddr userNodeToUserPasswordAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userPasswordAddr);
  ScAddr userNodeToUserSurnameAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userSurnameAddr);
  ScAddr userNodeToUserNameAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userNameAddr);
  ScAddr userNodeToUserPatronymicAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userPatronymicAddr);
  ScAddr userNodeToUserBirthdayAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userBirthdayAddr);
  ScAddr userNodeToUserAddressAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, userAddressAddr);
  ScAddr userNodeToUserGenderAddr = m_context.GenerateConnector(ScType::ConstCommonArc, userAddr, genderAddr);

  // Ребра от узлов отношений к ребрам
  ScAddr userLoginNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_LOGIN, userNodeToUserLoginAddr);
  ScAddr userPasswordNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_PASSWORD, userNodeToUserPasswordAddr);
  ScAddr userSurnameNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_SURNAME, userNodeToUserSurnameAddr);
  ScAddr userNameNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_NAME, userNodeToUserNameAddr);
  ScAddr userPatronymicNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_PATRONYMIC, userNodeToUserPatronymicAddr);
  ScAddr userBirthdayNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_BIRTHDATE, userNodeToUserBirthdayAddr);
  ScAddr userAddressNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_ADDRESS, userNodeToUserAddressAddr);
  ScAddr userGenderNonRoleAddr = m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::NREL_USER_GENDER, userNodeToUserGenderAddr);

  return action.FinishSuccessfully();
}


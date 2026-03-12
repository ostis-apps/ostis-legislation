#include "VerificationAgent.hpp"
#include "keynodes/RegistrationKeynodes.hpp"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <sstream>
#include <vector>
#include <unistd.h>

ScAddr VerificationAgent::GetActionClass() const
{
    return RegistrationKeynodes::action_verification;
}

ScResult VerificationAgent::DoProgram(ScAction & action)
{
    std::cout << "=== VerificationAgent started ===" << std::endl;
    
    // Проверяем, есть ли токен для верификации (rrel_2)
    ScTemplate tokenCheckTemplate;
    tokenCheckTemplate.Triple(
        action,
        ScType::VarPermPosArc >> "_token_arc",
        ScType::VarNodeLink >> "_token"
    );
    tokenCheckTemplate.Triple(
        RegistrationKeynodes::rrel_2,
        ScType::VarPermPosArc,
        "_token_arc"
    );
    
    ScTemplateSearchResult tokenCheckResult;
    bool hasToken = m_context.SearchByTemplate(tokenCheckTemplate, tokenCheckResult) && tokenCheckResult.Size() > 0;
    
    if (hasToken)
    {
        std::cout << "Режим проверки токена" << std::endl;
        return VerifyToken(action, tokenCheckResult[0]);
    }
    else
    {
        std::cout << "Режим отправки токена" << std::endl;
        return SendToken(action);
    }
}

ScResult VerificationAgent::SendToken(ScAction & action)
{
    std::cout << "=== SendToken started ===" << std::endl;
    
    // Ищем email в аргументах действия
    ScTemplate emailTemplate;
    emailTemplate.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_email"
    );
    emailTemplate.Triple(
        RegistrationKeynodes::rrel_1,
        ScType::VarPermPosArc,
        "_arc"
    );
    
    ScTemplateSearchResult emailResult;
    if (!m_context.SearchByTemplate(emailTemplate, emailResult) || emailResult.Size() == 0)
    {
        std::cout << "Email не найден в аргументах действия" << std::endl;
        return action.FinishUnsuccessfully();
    }
    
    ScTemplateResultItem item = emailResult[0];
    ScAddr emailLink = item["_email"];
    std::string email;
    
    // Получаем содержимое ссылки
    ScStreamPtr stream = m_context.GetLinkContent(emailLink);
    if (!stream || stream->Size() == 0)
    {
        std::cout << "Не удалось прочитать email из ссылки" << std::endl;
        return action.FinishUnsuccessfully();
    }
    
    std::string result;
    result.resize(stream->Size());
    size_t readBytes = 0;
    stream->Read(&result[0], stream->Size(), readBytes);
    email = result;
    
    std::cout << "Обработка email: " << email << std::endl;
    
    // Генерируем токен (6-значный код)
    std::string token = GenerateToken();
    std::cout << "Сгенерирован токен: " << token << std::endl;
    
    // ИЗМЕНЕНО: Ищем пользователя по nrel_system_identifier вместо nrel_email
    ScTemplate userTemplate;
    userTemplate.Triple(
        RegistrationKeynodes::concept_user,
        ScType::VarPermPosArc,
        ScType::VarNode >> "_user"
    );
    userTemplate.Quintuple(
        "_user",
        ScType::VarCommonArc >> "_email_arc",
        ScType::VarNodeLink >> "_user_email",
        ScType::VarPermPosArc,
        RegistrationKeynodes::nrel_system_identifier  // ИЗМЕНЕНО
    );
    
    ScTemplateSearchResult userResult;
    if (!m_context.SearchByTemplate(userTemplate, userResult))
    {
        std::cout << "Пользователи не найдены в системе" << std::endl;
        // НЕ ЗАВЕРШАЕМ - просто отправляем токен без привязки к пользователю
    }
    
    bool userFound = false;
    ScAddr userNode;
    
    // Ищем пользователя с нужным email
    for (size_t i = 0; i < userResult.Size(); i++)
    {
        ScTemplateResultItem userItem = userResult[i];
        ScAddr userEmailLink = userItem["_user_email"];
        ScStreamPtr userEmailStream = m_context.GetLinkContent(userEmailLink);
        
        if (userEmailStream && userEmailStream->Size() > 0)
        {
            std::string userEmailStr;
            userEmailStr.resize(userEmailStream->Size());
            size_t readBytes = 0;
            userEmailStream->Read(&userEmailStr[0], userEmailStream->Size(), readBytes);
            
            if (userEmailStr == email)
            {
                userNode = userItem["_user"];
                userFound = true;
                std::cout << "Найден пользователь для email: " << email << std::endl;
                break;
            }
        }
    }
    
    // Если пользователь найден - сохраняем токен
    if (userFound)
    {
        // Сохраняем токен для пользователя
        ScAddr tokenLink = m_context.GenerateLink(ScType::ConstNodeLink);
        m_context.SetLinkContent(tokenLink, token);
        
        // userNode -> tokenLink
        ScAddr tokenArc = m_context.GenerateConnector(ScType::ConstCommonArc, userNode, tokenLink);
        
        // nrel_verification_token -> (userNode -> tokenLink)
        m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_verification_token, tokenArc);
        
        std::cout << "Токен сохранен для пользователя" << std::endl;
    }
    else
    {
        std::cout << "Пользователь не найден, но продолжаем отправку email" << std::endl;
    }
    
    // Отправляем токен по email В ЛЮБОМ СЛУЧАЕ
    bool emailSent = SendEmail(email, token);
    
    if (!emailSent)
    {
        std::cout << "Не удалось отправить email на " << email << std::endl;
        return action.FinishUnsuccessfully();
    }
    
    std::cout << "Токен успешно отправлен на " << email << std::endl;
    
    // Создаем результат успешной отправки
    ScAddr resultLink = m_context.CreateLink(ScType::LinkConst);
    m_context.SetLinkContent(resultLink, "verification_token_sent");
    ScAddr resultArc = m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, action, resultLink);
    m_context.CreateEdge(ScType::EdgeAccessConstPosPerm, RegistrationKeynodes::nrel_result, resultArc);
    
    return action.FinishSuccessfully();
}

ScResult VerificationAgent::VerifyToken(ScAction & action, ScTemplateResultItem item)
{
    std::cout << "=== VerifyToken started ===" << std::endl;
    
    // Ищем email в аргументах действия
    ScTemplate emailTemplate;
    emailTemplate.Triple(
        action,
        ScType::VarPermPosArc >> "_email_arc",
        ScType::VarNodeLink >> "_email"
    );
    emailTemplate.Triple(
        RegistrationKeynodes::rrel_1,
        ScType::VarPermPosArc,
        "_email_arc"
    );
    
    ScTemplateSearchResult emailResult;
    if (!m_context.SearchByTemplate(emailTemplate, emailResult) || emailResult.Size() == 0)
    {
        std::cout << "Email не найден для проверки токена" << std::endl;
        return action.FinishUnsuccessfully();
    }
    
    ScAddr emailLink = emailResult[0]["_email"];
    ScAddr tokenLink = item["_token"];
    std::string email, token;
    
    // Читаем email
    ScStreamPtr emailStream = m_context.GetLinkContent(emailLink);
    if (emailStream && emailStream->Size() > 0)
    {
        std::string emailStr;
        emailStr.resize(emailStream->Size());
        size_t readBytes = 0;
        emailStream->Read(&emailStr[0], emailStream->Size(), readBytes);
        email = emailStr;
    }
    
    // Читаем токен
    ScStreamPtr tokenStream = m_context.GetLinkContent(tokenLink);
    if (tokenStream && tokenStream->Size() > 0)
    {
        std::string tokenStr;
        tokenStr.resize(tokenStream->Size());
        size_t readBytes = 0;
        tokenStream->Read(&tokenStr[0], tokenStream->Size(), readBytes);
        token = tokenStr;
    }
    
    std::cout << "Проверка токена для email: " << email << ", токен: " << token << std::endl;
    
    // ИЗМЕНЕНО: Ищем пользователя по nrel_system_identifier
    ScTemplate userTemplate;
    userTemplate.Triple(
        RegistrationKeynodes::concept_user,
        ScType::VarPermPosArc,
        ScType::VarNode >> "_user"
    );
    userTemplate.Quintuple(
        "_user",
        ScType::VarCommonArc >> "_email_arc",
        ScType::VarNodeLink >> "_user_email",
        ScType::VarPermPosArc,
        RegistrationKeynodes::nrel_system_identifier  // ИЗМЕНЕНО
    );
    
    ScTemplateSearchResult userResult;
    if (!m_context.SearchByTemplate(userTemplate, userResult))
    {
        std::cout << "Пользователь с email " << email << " не найден" << std::endl;
        return action.FinishUnsuccessfully();
    }
    
    // Проверяем каждого пользователя
    for (size_t i = 0; i < userResult.Size(); i++)
    {
        ScTemplateResultItem userItem = userResult[i];
        std::string userEmail;
        
        ScAddr userEmailLink = userItem["_user_email"];
        ScStreamPtr userEmailStream = m_context.GetLinkContent(userEmailLink);
        
        if (userEmailStream && userEmailStream->Size() > 0)
        {
            std::string userEmailStr;
            userEmailStr.resize(userEmailStream->Size());
            size_t readBytes = 0;
            userEmailStream->Read(&userEmailStr[0], userEmailStream->Size(), readBytes);
            userEmail = userEmailStr;
        }
        
        if (userEmail == email)
        {
            // Нашли пользователя с таким email
            ScAddr userNode = userItem["_user"];
            
            // Ищем токен пользователя
            ScTemplate tokenTemplate;
            tokenTemplate.Quintuple(
                userNode,
                ScType::VarCommonArc >> "_token_arc",
                ScType::VarNodeLink >> "_user_token",
                ScType::VarPermPosArc,
                RegistrationKeynodes::nrel_verification_token
            );
            
            ScTemplateSearchResult tokenSearchResult;
            m_context.SearchByTemplate(tokenTemplate, tokenSearchResult);
            
            std::cout << "Найдено токенов для пользователя: " << tokenSearchResult.Size() << std::endl;
            
            if (tokenSearchResult.Size() > 0)
            {
                ScTemplateResultItem tokenItem = tokenSearchResult[0];
                std::string userToken;
                
                ScAddr userTokenLink = tokenItem["_user_token"];
                ScStreamPtr userTokenStream = m_context.GetLinkContent(userTokenLink);
                
                if (userTokenStream && userTokenStream->Size() > 0)
                {
                    std::string userTokenStr;
                    userTokenStr.resize(userTokenStream->Size());
                    size_t readBytes = 0;
                    userTokenStream->Read(&userTokenStr[0], userTokenStream->Size(), readBytes);
                    userToken = userTokenStr;
                }
                
                std::cout << "Найден токен пользователя: " << userToken << std::endl;
                
                if (userToken == token)
                {
                    // Токен совпал - отмечаем как верифицированного
                    m_context.GenerateConnector(
                        ScType::ConstPermPosArc,
                        RegistrationKeynodes::concept_verified_user,
                        userNode
                    );
                    
                    // Удаляем токен верификации
                    ScAddr tokenArc = tokenItem["_token_arc"];
                    m_context.EraseElement(tokenArc);
                    m_context.EraseElement(userTokenLink);
                    
                    std::cout << "Email успешно подтвержден для: " << email << std::endl;
                    
                    // Создаем результат успешной верификации
                    ScAddr resultLink = m_context.GenerateLink(ScType::ConstNodeLink);
                    m_context.SetLinkContent(resultLink, "EmailVerified");
                    ScAddr resultArc = m_context.GenerateConnector(ScType::ConstPermPosArc, action, resultLink);
                    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_result, resultArc);
                    
                    return action.FinishSuccessfully();
                }
                else
                {
                    std::cout << "Токены не совпадают. Ожидаемый: " << userToken << ", полученный: " << token << std::endl;
                }
            }
            else
            {
                std::cout << "Токен не найден для пользователя" << std::endl;
            }
            
            break;
        }
    }
    
    std::cout << "Неверный токен для email: " << email << std::endl;
    return action.FinishUnsuccessfully();
}

std::string VerificationAgent::GenerateToken()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(100000, 999999);
    return std::to_string(distrib(gen));
}

bool VerificationAgent::SendEmail(const std::string & email, const std::string & token)
{
    SC_LOG_INFO("=== VerificationAgent::SendEmail ===");
    SC_LOG_INFO("Email: " << email << ", Token: " << token);
    
    // Список возможных путей для поиска скрипта
    std::vector<std::string> possiblePaths = {
        "problem-solver/py/send_token.py",
        "../problem-solver/py/send_token.py",
        "../../problem-solver/py/send_token.py"
    };
    
    const char* homeDir = getenv("HOME");
    const char* userName = getenv("USER");
    
    if (homeDir != nullptr) {
        possiblePaths.push_back(std::string(homeDir) + "/ostis-legislation/problem-solver/py/send_token.py");
        possiblePaths.push_back(std::string(homeDir) + "/sc-legislation-app/problem-solver/py/send_token.py");
    }
    
    if (userName != nullptr) {
        possiblePaths.push_back("/home/" + std::string(userName) + "/ostis-legislation/problem-solver/py/send_token.py");
        possiblePaths.push_back("/home/" + std::string(userName) + "/sc-legislation-app/problem-solver/py/send_token.py");
    }
    
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        SC_LOG_INFO("Текущая директория: " << cwd);
        possiblePaths.insert(possiblePaths.begin(), std::string(cwd) + "/problem-solver/py/send_token.py");
    }
    
    std::string scriptPath;
    bool found = false;
    
    SC_LOG_INFO("Поиск скрипта send_token.py...");
    for (const auto& path : possiblePaths) {
        std::ifstream testFile(path);
        if (testFile.good()) {
            scriptPath = path;
            found = true;
            testFile.close();
            SC_LOG_INFO("Скрипт найден: " << scriptPath);
            break;
        }
    }
    
    if (!found) {
        SC_LOG_ERROR("Скрипт send_token.py не найден");
        return false;
    }
    
    std::string command = "python3 \"" + scriptPath + "\" \"" + email + "\" \"" + token + "\" 2>&1";
    SC_LOG_INFO("Выполнение команды: " << command);
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        SC_LOG_ERROR("Ошибка выполнения команды");
        return false;
    }
    
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
        std::cout << buffer;
    }
    
    int status = pclose(pipe);
    SC_LOG_INFO("Код выхода: " << status);
    
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0) {
            SC_LOG_INFO("Email успешно отправлен на " << email);
            return true;
        } else {
            SC_LOG_ERROR("Ошибка отправки email, код выхода: " << exit_code);
            return false;
        }
    } else {
        SC_LOG_ERROR("Команда завершилась аварийно");
        return false;
    }
}

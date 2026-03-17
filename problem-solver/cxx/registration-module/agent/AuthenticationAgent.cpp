#include "AuthenticationAgent.hpp"
#include "keynodes/RegistrationKeynodes.hpp"

ScAddr AuthenticationAgent::GetActionClass() const
{
    return RegistrationKeynodes::action_authentication;
}

ScResult AuthenticationAgent::DoProgram(ScAction & action)
{
    SC_LOG_INFO("Authentication agent started");
    
    // Получаем email (rrel_1)
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
        SC_LOG_ERROR("Email not found");
        return action.FinishUnsuccessfully();
    }
    
    ScAddr emailLink = emailResult[0]["_email"];
    std::string email;
    
    // Читаем email
    ScStreamPtr emailStream = m_context.GetLinkContent(emailLink);
    if (emailStream && emailStream->Size() > 0)
    {
        email.resize(emailStream->Size());
        size_t readBytes = 0;
        emailStream->Read(&email[0], emailStream->Size(), readBytes);
    }
    
    SC_LOG_INFO("Authenticating user: " << email);
    
    // Получаем password (rrel_2)
    ScTemplate passwordTemplate;
    passwordTemplate.Triple(
        action,
        ScType::VarPermPosArc >> "_arc",
        ScType::VarNodeLink >> "_password"
    );
    passwordTemplate.Triple(
        RegistrationKeynodes::rrel_2,
        ScType::VarPermPosArc,
        "_arc"
    );
    
    ScTemplateSearchResult passwordResult;
    if (!m_context.SearchByTemplate(passwordTemplate, passwordResult) || passwordResult.Size() == 0)
    {
        SC_LOG_ERROR("Password not found");
        return action.FinishUnsuccessfully();
    }
    
    ScAddr passwordLink = passwordResult[0]["_password"];
    std::string password;
    
    // Читаем password
    ScStreamPtr passwordStream = m_context.GetLinkContent(passwordLink);
    if (passwordStream && passwordStream->Size() > 0)
    {
        password.resize(passwordStream->Size());
        size_t readBytes = 0;
        passwordStream->Read(&password[0], passwordStream->Size(), readBytes);
    }
    
    // Ищем пользователя по nrel_system_identifier
    ScTemplate userTemplate;
    userTemplate.Triple(
        RegistrationKeynodes::concept_user,
        ScType::VarPermPosArc,
        ScType::VarNode >> "_user"
    );
    userTemplate.Quintuple(
        "_user",
        ScType::VarCommonArc,
        ScType::VarNodeLink >> "_user_email",
        ScType::VarPermPosArc,
        RegistrationKeynodes::nrel_system_identifier
    );
    
    ScTemplateSearchResult userResult;
    if (!m_context.SearchByTemplate(userTemplate, userResult))
    {
        SC_LOG_ERROR("No users found in system");
        CreateErrorResult(action, "User not found");
        return action.FinishUnsuccessfully();
    }
    
    // Ищем пользователя с нужным email
    for (size_t i = 0; i < userResult.Size(); i++)
    {
        ScAddr userEmailLink = userResult[i]["_user_email"];
        ScStreamPtr userEmailStream = m_context.GetLinkContent(userEmailLink);
        
        if (!userEmailStream || userEmailStream->Size() == 0)
            continue;
        
        std::string userEmail;
        userEmail.resize(userEmailStream->Size());
        size_t readBytes = 0;
        userEmailStream->Read(&userEmail[0], userEmailStream->Size(), readBytes);
        
        if (userEmail == email)
        {
            // Нашли пользователя - проверяем пароль
            ScAddr userNode = userResult[i]["_user"];
            
            // Получаем сохраненный пароль
            ScTemplate passwordTemplate;
            passwordTemplate.Quintuple(
                userNode,
                ScType::VarCommonArc,
                ScType::VarNodeLink >> "_stored_password",
                ScType::VarPermPosArc,
                RegistrationKeynodes::nrel_password
            );
            
            ScTemplateSearchResult passwordResult;
            if (!m_context.SearchByTemplate(passwordTemplate, passwordResult) || passwordResult.Size() == 0)
            {
                SC_LOG_ERROR("Password not found for user");
                CreateErrorResult(action, "Invalid credentials");
                return action.FinishUnsuccessfully();
            }
            
            ScAddr storedPasswordLink = passwordResult[0]["_stored_password"];
            ScStreamPtr storedPasswordStream = m_context.GetLinkContent(storedPasswordLink);
            
            if (!storedPasswordStream || storedPasswordStream->Size() == 0)
            {
                SC_LOG_ERROR("Cannot read stored password");
                CreateErrorResult(action, "Invalid credentials");
                return action.FinishUnsuccessfully();
            }
            
            std::string storedPassword;
            storedPassword.resize(storedPasswordStream->Size());
            size_t readBytes2 = 0;
            storedPasswordStream->Read(&storedPassword[0], storedPasswordStream->Size(), readBytes2);
            
            // Хешируем введенный пароль
            std::string hashedPassword = RegistrationKeynodes::HashPassword(password);
            
            if (hashedPassword == storedPassword)
            {
                SC_LOG_INFO("Authentication successful for: " << email);
                CreateSuccessResult(action, userNode);
                return action.FinishSuccessfully();
            }
            else
            {
                SC_LOG_ERROR("Password mismatch");
                CreateErrorResult(action, "Invalid credentials");
                return action.FinishUnsuccessfully();
            }
        }
    }
    
    SC_LOG_ERROR("User not found: " << email);
    CreateErrorResult(action, "User not found");
    return action.FinishUnsuccessfully();
}

void AuthenticationAgent::CreateSuccessResult(ScAction & action, ScAddr userNode)
{
    ScAddr resultNode = m_context.GenerateNode(ScType::ConstNode);
    ScAddr statusLink = m_context.GenerateLink();
    m_context.SetLinkContent(statusLink, "Valid");
    
    ScAddr arc1 = m_context.GenerateConnector(ScType::ConstCommonArc, resultNode, statusLink);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_status, arc1);
    
    ScAddr arc2 = m_context.GenerateConnector(ScType::ConstCommonArc, resultNode, userNode);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_result, arc2);
    
    ScAddr resultArc = m_context.GenerateConnector(ScType::ConstPermPosArc, action, resultNode);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_result, resultArc);
}

void AuthenticationAgent::CreateErrorResult(ScAction & action, std::string const & message)
{
    ScAddr resultNode = m_context.GenerateNode(ScType::ConstNode);
    ScAddr statusLink = m_context.GenerateLink();
    m_context.SetLinkContent(statusLink, "Invalid");
    
    ScAddr messageLink = m_context.GenerateLink();
    m_context.SetLinkContent(messageLink, message);
    
    ScAddr arc1 = m_context.GenerateConnector(ScType::ConstCommonArc, resultNode, statusLink);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_status, arc1);
    
    ScAddr arc2 = m_context.GenerateConnector(ScType::ConstCommonArc, resultNode, messageLink);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_error_message, arc2);
    
    ScAddr resultArc = m_context.GenerateConnector(ScType::ConstPermPosArc, action, resultNode);
    m_context.GenerateConnector(ScType::ConstPermPosArc, RegistrationKeynodes::nrel_result, resultArc);
}

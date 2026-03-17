#pragma once

#include <sc-memory/sc_agent.hpp>

#include "keynodes/RegistrationKeynodes.hpp"

class VerificationAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;
    ScResult DoProgram(ScAction & action) override;

private:
    ScResult SendToken(ScAction & action);
    ScResult VerifyToken(ScAction & action, ScTemplateResultItem item);
    std::string GenerateToken();
    bool SendEmail(const std::string& email, const std::string& token);
};
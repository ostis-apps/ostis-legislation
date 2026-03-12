#pragma once

#include <sc-memory/sc_agent.hpp>
#include "VerificationAgent.hpp"

class AuthenticationAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  
  ScResult DoProgram(ScAction & action) override;

private:
  void CreateSuccessResult(ScAction & action, ScAddr userNode);
  void CreateErrorResult(ScAction & action, std::string const & message);
};

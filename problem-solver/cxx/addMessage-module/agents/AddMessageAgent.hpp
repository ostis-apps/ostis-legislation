#pragma once

#include <sc-memory/sc_agent.hpp>

namespace addMessageModule
{

class AddMessageAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

}

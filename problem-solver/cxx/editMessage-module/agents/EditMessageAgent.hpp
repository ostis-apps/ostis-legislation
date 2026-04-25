#pragma once

#include <sc-memory/sc_agent.hpp>

namespace editMessageModule
{

class EditMessageAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

} // namespace editMessageModule
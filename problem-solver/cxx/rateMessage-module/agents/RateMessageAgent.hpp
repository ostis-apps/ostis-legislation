#pragma once
#include <sc-memory/sc_agent.hpp>

namespace rateMessageModule
{

class RateMessageAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

} // namespace rateMessageModule

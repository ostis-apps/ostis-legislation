#pragma once

#include <sc-memory/sc_agent.hpp>

class RatingUpdateAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

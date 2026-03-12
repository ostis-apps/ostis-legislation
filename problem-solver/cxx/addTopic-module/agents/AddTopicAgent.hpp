#pragma once

#include <sc-memory/sc_agent.hpp>

namespace addTopicModule
{

class AddTopicAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

}

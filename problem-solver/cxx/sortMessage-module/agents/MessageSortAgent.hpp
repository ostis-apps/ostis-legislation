#pragma once
#include <sc-memory/sc_agent.hpp>

namespace sortMessageModule
{

class SortMessageAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;
    ScResult DoProgram(ScAction & action) override;

private:
    int GetRating(ScAddr const & messageAddr);
};

} // namespace sortMessageModule

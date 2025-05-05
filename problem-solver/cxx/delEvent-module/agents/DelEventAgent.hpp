#pragma once

#include <sc-memory/sc_agent.hpp>

class DelEventAgent : public ScActionInitiatedAgent {
public:
    ScAddr GetActionClass() const override;
    ScResult DoProgram(ScAction & action) override;
    void remove_event_link(ScAddr const & userAddr, std::string const & eventName);
};

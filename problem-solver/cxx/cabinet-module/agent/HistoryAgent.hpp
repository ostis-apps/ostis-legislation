#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/CabinetKeynodes.hpp"

class ScAddHistoryEntryAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  bool create_history_entry(
    ScAddr const & user_node,
    ScAddr const & text_link,
    ScAddr const & date_link,
    ScAddr const & article_node,
    ScStructure & result);

  void create_success_result(ScAddr const & history_node, ScStructure & result);
  void create_error_result(std::string const & message, ScStructure & result);
};
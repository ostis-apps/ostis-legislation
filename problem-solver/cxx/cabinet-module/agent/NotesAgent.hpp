#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/CabinetKeynodes.hpp"

class ScAddNoteAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  bool create_note(
    ScAddr const & user_node,
    ScAddr const & article_node,
    ScAddr const & text_link,
    ScAddr const & created_link,
    ScAddr const & updated_link,
    ScStructure & result);

  void create_success_result(ScAddr const & note_node, ScStructure & result);
  void create_error_result(std::string const & message, ScStructure & result);
};
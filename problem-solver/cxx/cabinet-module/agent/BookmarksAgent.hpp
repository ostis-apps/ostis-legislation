#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/CabinetKeynodes.hpp"

class ScAddBookmarkAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  bool create_bookmark(
    ScAddr const & user_node,
    ScAddr const & article_node,
    ScAddr const & tags_link,
    ScAddr const & date_link,
    ScStructure & result);

  void create_success_result(ScAddr const & bookmark_node, ScStructure & result);
  void create_error_result(std::string const & message, ScStructure & result);

  std::string readStringFromLink(ScAddr const & linkAddr);
};
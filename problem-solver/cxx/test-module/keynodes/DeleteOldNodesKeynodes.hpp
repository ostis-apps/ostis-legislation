#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_memory.hpp>

namespace utils
{

class DeleteOldNodesKeynodes : public ScObject
{
public:
  static inline ScAddr action_delete_old_nodes;
  static inline ScAddr nrel_asked_questions;
  static inline ScAddr nrel_selected_answers;

  static void Initialize(ScMemoryContext * context)
  {
    if (!action_delete_old_nodes.IsValid())
    {
      action_delete_old_nodes = context->SearchElementBySystemIdentifier("action_delete_old_nodes");
      nrel_asked_questions = context->SearchElementBySystemIdentifier("nrel_asked_questions");
      nrel_selected_answers = context->SearchElementBySystemIdentifier("nrel_selected_answers");
    }
  }
};

}  // namespace utils

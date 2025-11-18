#pragma once

#include <sc-memory/sc_keynodes.hpp>

class DeleteOldNodesKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_delete_old_nodes{"action_delete_old_nodes", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_asked_questions{"nrel_asked_questions", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_selected_answers{"nrel_selected_answers", ScType::ConstNodeNonRole};
};

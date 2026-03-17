/*
 * This source file is part of an OSTIS project. For the latest info, see
 * [http://ostis.net](http://ostis.net) Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_keynodes.hpp>

class Keynodes : public ScKeynodes
{
public:
  // Test actions
  static inline ScKeynode const action_choice_next_question{"action_choice_next_question", ScType::ConstNodeClass};
  static inline ScKeynode const action_search_answers_for_question{"action_search_answers_for_question", ScType::ConstNodeClass};
  static inline ScKeynode const action_save_answer{"action_save_answer", ScType::ConstNodeClass};
  static inline ScKeynode const action_check_answer{"action_check_answer", ScType::ConstNodeClass};
  static inline ScKeynode const action_delete_old_nodes{"action_delete_old_nodes", ScType::ConstNodeClass};
  static inline ScKeynode const action_update_rating{"action_update_rating", ScType::ConstNodeClass};
  
  // Old keynodes (можно удалить если не используются)
  static inline ScKeynode const action_search_subdividing{"action_search_subdividing", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_subdividing{"nrel_subdividing", ScType::ConstNodeNonRole};
  static inline ScKeynode const action_search_isomorphic_structures{"action_search_isomorphic_structures", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_search_result{"nrel_search_result", ScType::ConstNodeNonRole};
  static inline ScKeynode const empty_set{"empty_set", ScType::ConstNodeClass};
  static inline ScKeynode const action_search_minimum_path{"action_search_minimum_path", ScType::ConstNodeClass};
  static inline ScKeynode const action_example_inference{"action_example_inference", ScType::ConstNodeClass};
};

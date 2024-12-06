#pragma once

#include <sc-memory/sc_keynodes.hpp>

class GenerateRecommendationsKeynodes : public ScKeynodes {
public:
    static inline ScKeynode const action_generate_recommendations{"action_generate_recommendations", ScType::NodeConstClass};
    static inline ScKeynode const nrel_user_answer{"nrel_user_answer", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_generated_question_incorrect_answer{"nrel_generated_question_incorrect_answer", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_generated_question_correct_answer{"nrel_generated_question_correct_answer", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_test_recommendations{"nrel_test_recommendations", ScType::ConstNodeNonRole};
};
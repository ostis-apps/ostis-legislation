#pragma once

#include <sc-memory/sc_keynodes.hpp>

class GenerateIncorrectAnswersKeynodes : public ScKeynodes {
public:
    static inline ScKeynode const action_generate_incorrect_answers{"action_generate_incorrect_answers", ScType::NodeConstClass};
    static inline ScKeynode const nrel_generated_question_incorrect_answer{"nrel_generated_question_incorrect_answer", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_related_term{"nrel_related_term", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_generated_question_model{"nrel_generated_question_model", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_answer_struct{"nrel_answer_struct", ScType::NodeConstNoRole};
};
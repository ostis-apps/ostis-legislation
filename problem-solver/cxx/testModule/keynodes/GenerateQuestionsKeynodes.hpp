#pragma once

#include <sc-memory/sc_keynodes.hpp>

class GenerateQuestionsKeynodes : public ScKeynodes {
public:
    static inline ScKeynode const action_generate_questions{"action_generate_questions", ScType::NodeConstClass};
    static inline ScKeynode const nrel_generated_question{"nrel_generated_question", ScType::NodeConstNoRole};
    static inline ScKeynode const define_model{"define_model", ScType::NodeConst};
    static inline ScKeynode const question_model{"question_model", ScType::NodeConstClass};
    static inline ScKeynode const nrel_question_struct{"nrel_question_struct", ScType::NodeConstNoRole};
    static inline ScKeynode const nrel_answer_struct{"nrel_answer_struct", ScType::NodeConstNoRole};
    static inline ScKeynode const nrel_question_text{"nrel_question_text", ScType::NodeConstNoRole};
    static inline ScKeynode const generated_question{"generated_question", ScType::NodeConstClass};
    static inline ScKeynode const nrel_generated_question_correct_answer{"nrel_generated_question_correct_answer", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_generated_question_text{"nrel_generated_question_text", ScType::ConstNodeNonRole};
    static inline ScKeynode const test{"test", ScType::ConstNodeClass};
    static inline ScKeynode const nrel_generated_question_model{"nrel_generated_question_model", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_question_related_term{"nrel_question_related_term", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_related_term{"nrel_related_term", ScType::ConstNodeNonRole};
};
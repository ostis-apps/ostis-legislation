#pragma once
#include <sc-memory/sc_keynodes.hpp>

namespace sortMessageModule
{

class Keynodes
{
public:
    static inline ScKeynode const action_sort_message{
        "action_sort_message", ScType::ConstNodeClass};

    static inline ScKeynode const nrel_message_date{
        "nrel_message_date", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_likes{
        "nrel_likes", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_dislikes{
        "nrel_dislikes", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_best_message{
        "nrel_best_message", ScType::ConstNodeNonRole};

    static inline ScKeynode const concept_sort_by_date{
        "concept_sort_by_date", ScType::ConstNodeClass};
    static inline ScKeynode const concept_sort_by_rating{
        "concept_sort_by_rating", ScType::ConstNodeClass};
    static inline ScKeynode const concept_sort_by_relevance{
        "concept_sort_by_relevance", ScType::ConstNodeClass};

    static inline ScKeynode const rrel_1{"rrel_1", ScType::ConstNodeRole};
    static inline ScKeynode const rrel_2{"rrel_2", ScType::ConstNodeRole};
};

} // namespace sortMessageModule

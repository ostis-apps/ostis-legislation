#include "TestModule.hpp"

#include "agents/GenerateQuestionsAgent.hpp"
#include "agents/GenerateIncorrectAnswersAgent.hpp"
#include "agents/GenerateRecommendationsAgent.hpp"

SC_MODULE_REGISTER(TestModule)
    ->Agent<GenerateQuestionsAgent>()
    ->Agent<GenerateIncorrectAnswersAgent>()
    ->Agent<GenerateRecommendationsAgent>();

#include "TestModule.hpp"

#include "agents/GenerateQuestionsAgent.hpp"
#include "agents/GenerateIncorrectAnswersAgent.hpp"

SC_MODULE_REGISTER(TestModule)
->Agent<GenerateQuestionsAgent>()
->Agent<GenerateIncorrectAnswersAgent>();

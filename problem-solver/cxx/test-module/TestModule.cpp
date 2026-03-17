#include "TestModule.hpp"

#include "agents/ChoiceNextQuestionAgent.hpp"
#include "agents/SaveAnswerAgent.hpp"
#include "agents/SearchAnswersForQuestionAgent.hpp"
#include "agents/CheckTheAnswerAgent.hpp"
#include "agents/RatingUpdateAgent.hpp"
#include "agents/DeleteOldNodesAgent.hpp"

using namespace utils;

SC_MODULE_REGISTER(TestModule)
  ->Agent<ChoiceNextQuestionAgent>()
  ->Agent<SaveAnswerAgent>()
  ->Agent<SearchAnswersForQuestionAgent>()
  ->Agent<CheckTheAnswerAgent>()
  ->Agent<RatingUpdateAgent>()
  ->Agent<DeleteOldNodesAgent>();

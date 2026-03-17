#include "AddTopicModule.hpp"
#include "agents/AddTopicAgent.hpp"

using namespace addTopicModule;

SC_MODULE_REGISTER(AddTopicModule)
  ->Agent<AddTopicAgent>();

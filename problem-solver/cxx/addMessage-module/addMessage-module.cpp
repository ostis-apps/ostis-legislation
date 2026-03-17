#include "addMessage-module.hpp"
#include "agents/AddMessageAgent.hpp"

using namespace addMessageModule;

SC_MODULE_REGISTER(AddMessageModule)
  ->Agent<AddMessageAgent>();

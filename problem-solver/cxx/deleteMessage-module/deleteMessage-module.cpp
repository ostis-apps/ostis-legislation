#include "deleteMessage-module.hpp"
#include "agents/DeleteMessageAgent.hpp"

using namespace deleteMessageModule;

SC_MODULE_REGISTER(DeleteMessageModule)
  ->Agent<DeleteMessageAgent>();

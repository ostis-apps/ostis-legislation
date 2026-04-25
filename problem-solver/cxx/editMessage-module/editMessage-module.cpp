#include "editMessage-module.hpp"
#include "agents/EditMessageAgent.hpp"

SC_MODULE_REGISTER(EditMessageModule)
  ->Agent<editMessageModule::EditMessageAgent>();
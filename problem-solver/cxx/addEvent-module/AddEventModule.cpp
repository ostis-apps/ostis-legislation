#include "AddEventModule.hpp"

#include "agents/AddEventAgent.hpp"

SC_MODULE_REGISTER(AddEventModule)
  ->Agent<AddEventAgent>();
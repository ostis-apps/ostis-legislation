#include "DelEventModule.hpp"

#include "agents/DelEventAgent.hpp"

SC_MODULE_REGISTER(DelEventModule)
  ->Agent<DelEventAgent>();
#include "user_request_module.hpp"

#include "agent/user_request_agent.hpp"

SC_MODULE_REGISTER(ScRequestModule)
  ->Agent<ScUserRequestAgent>();
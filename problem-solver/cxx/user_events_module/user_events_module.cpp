#include "user_events_module.hpp"

#include "agent/user_events_agent.hpp"

SC_MODULE_REGISTER(ScEventModule)
  ->Agent<ScUserEventsAgent>();

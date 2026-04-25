#include "rateMessage-module.hpp"
#include "agents/RateMessageAgent.hpp"

using namespace rateMessageModule;

SC_MODULE_REGISTER(RateMessageModule)
  ->Agent<RateMessageAgent>();

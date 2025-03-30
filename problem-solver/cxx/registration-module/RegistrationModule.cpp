#include "RegistrationModule.hpp"

#include "agents/RegistrationAgent.hpp"

SC_MODULE_REGISTER(RegistrationModule)
  ->Agent<RegistrationAgent>();

#include "RegistrationModule.hpp"

#include "agents/AuthenticationAgent.hpp"

SC_MODULE_REGISTER(RegistrationModule)
  ->Agent<AuthenticationAgent>();

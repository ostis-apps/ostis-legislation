#include "RegistrationModule.hpp"

#include <sc-memory/sc_agent.hpp>
#include "agent/registration_agent.hpp"
#include "agent/VerificationAgent.hpp"
#include "agent/AuthenticationAgent.hpp"

SC_MODULE_REGISTER(RegistrationModule)
  ->Agent<ScRegistrationAgent>()
  ->Agent<VerificationAgent>()
  ->Agent<AuthenticationAgent>();

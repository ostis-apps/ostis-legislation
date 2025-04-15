#pragma once

#include <sc-memory/sc_agent.hpp>

#include <openssl/sha.h>


class AuthenticationAgent : public ScActionInitiatedAgent {
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};
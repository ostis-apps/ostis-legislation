#pragma once

#include <sc-memory/sc_module.hpp>

class RegistrationModule : public ScModule
{
public:
  RegistrationModule() = default;
  ~RegistrationModule() override = default;

  void Initialize(ScMemoryContext * ctx) override {}
  void Shutdown(ScMemoryContext * ctx) override {}
};

#pragma once

#include <sc-memory/sc_module.hpp>

class CabinetModule : public ScModule
{
public:
  CabinetModule() = default;
  ~CabinetModule() override = default;

  void Initialize(ScMemoryContext * ctx) override {}
  void Shutdown(ScMemoryContext * ctx) override {}
};
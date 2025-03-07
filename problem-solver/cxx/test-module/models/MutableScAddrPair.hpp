#pragma once

#include "sc-memory/sc_addr.hpp"

class MutableScAddrPair
{
private:
  ScAddr first;
  ScAddr second;

public:
  MutableScAddrPair(ScAddr first, ScAddr second)
    : first(first)
    , second(second)
  {
  }

  ScAddr getFirst();
  ScAddr getSecond();
  bool operator==(MutableScAddrPair const & other) const;
  bool operator<(MutableScAddrPair const & other) const;
};
#include "SearchModule.hpp"

#include "agents/SearchAgent.hpp"

SC_MODULE_REGISTER(SearchModule)
  ->Agent<SearchAgent>();

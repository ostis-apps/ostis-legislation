#include "MessageSortModule.hpp"
#include "agents/MessageSortAgent.hpp"

using namespace sortMessageModule;

SC_MODULE_REGISTER(SortMessageModule)
    ->Agent<SortMessageAgent>();

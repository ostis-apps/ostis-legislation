#include "CabinetModule.hpp"

#include <sc-memory/sc_agent.hpp>

#include "agent/BookmarksAgent.hpp"
#include "agent/HistoryAgent.hpp"
#include "agent/NotesAgent.hpp"

SC_MODULE_REGISTER(CabinetModule)
  ->Agent<ScAddBookmarkAgent>()
  ->Agent<ScAddHistoryEntryAgent>()
  ->Agent<ScAddNoteAgent>();
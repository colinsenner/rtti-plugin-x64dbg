#pragma once

#include "..\plugin.h"
#include <string>
#include <unordered_map>
#include <vector>

// Fixed-capacity FIFO cache mapping address -> demangled RTTI class name.
// An empty string is a valid cached value meaning "checked, not a C++ object".
//
// Once full, inserting a new address evicts whichever entry was inserted
// longest ago, so callers never need to pick a "safe" upper bound - the
// cache just recomputes evicted entries the next time they're queried.
//
// Not thread-safe: only call this from the debugger's plugin callback thread.
class RttiCache
{
  public:
    explicit RttiCache(size_t capacity = 64);

    // Returns true if addr is cached, filling outValue with the cached result
    // (which may be an empty string for a negative cache hit).
    bool TryGet(duint addr, std::string &outValue) const;

    // Inserts or updates the cached value for addr.
    void Insert(duint addr, std::string value);

    // Drops all cached entries. Call whenever the debuggee's memory may have
    // changed underneath us (process create/exit/attach/detach, step, resume).
    void Clear();

  private:
    struct Slot
    {
        duint addr = 0;
        std::string value;
        bool occupied = false;
    };

    std::vector<Slot> m_slots;
    std::unordered_map<duint, size_t> m_index;
    size_t m_head = 0;
};

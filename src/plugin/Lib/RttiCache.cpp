#include "RttiCache.h"

RttiCache::RttiCache(size_t capacity) : m_slots(capacity) {}

bool RttiCache::TryGet(duint addr, std::string &outValue) const
{
    auto it = m_index.find(addr);
    if (it == m_index.end())
    {
        return false;
    }

    outValue = m_slots[it->second].value;
    return true;
}

void RttiCache::Insert(duint addr, std::string value)
{
    // Already cached (e.g. re-queried before eviction) - update in place,
    // don't touch insertion order.
    auto it = m_index.find(addr);
    if (it != m_index.end())
    {
        m_slots[it->second].value = std::move(value);
        return;
    }

    Slot &slot = m_slots[m_head];
    if (slot.occupied)
    {
        m_index.erase(slot.addr);
    }

    slot.addr = addr;
    slot.value = std::move(value);
    slot.occupied = true;

    m_index[addr] = m_head;

    m_head = (m_head + 1) % m_slots.size();
}

void RttiCache::Clear()
{
    m_index.clear();
    for (auto &slot : m_slots)
    {
        slot.occupied = false;
    }
    m_head = 0;
}

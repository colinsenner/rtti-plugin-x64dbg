#pragma once

#define ADDPTR(ptr, add) PVOID((PBYTE(ptr) + size_t(add)))
#define SUBPTR(ptr, sub) PVOID((PBYTE(ptr) - size_t(sub)))

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
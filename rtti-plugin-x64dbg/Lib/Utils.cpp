#pragma once
#include "..\plugin.h"

// When you have an address in the executable and you need to get the base address of the executable
duint GetModuleBaseAddress(duint addr)
{
	return ((addr << 32) >> 32);
}
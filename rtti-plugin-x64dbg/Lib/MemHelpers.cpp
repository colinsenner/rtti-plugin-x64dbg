#include "MemHelpers.h"

bool DbgDerefMemRead(duint addr, void* dest, duint size)
{
	duint val;

	if (!DbgMemRead(addr, &val, sizeof(val)))
		return false;

	if (!DbgMemRead(val, dest, size))
		return false;

	return true;
}
#pragma once
#include "macros.h"
#include "..\plugin.h"
#include <string>

using namespace std;

struct RTTICompleteObjectLocator;

struct vftable_t 
{
	RTTICompleteObjectLocator* pCompleteObjectLocator;		// offset -4

	void Print();
};

inline void vftable_t::Print()
{
	dprintf("  pCompleteObjectLocator: %p\n", pCompleteObjectLocator);
}

struct PMD
{
	// A vbtable (virtual base class table) is generated for multiple virtual inheritance.
	// Because it is sometimes necessary to upclass (casting to base classes), the exact location of
	// the base class needs to be determined.

	int mdisp;  //member displacement, vftable offset (if PMD.pdisp is -1)
	int pdisp;  //vbtable displacement, vbtable offset (-1: vftable is at displacement PMD.mdisp inside the class)
	int vdisp;  //displacement inside vbtable, displacement of the base class vftable pointer inside the vbtable

	void Print();
};

inline void PMD::Print() 
{
	dprintf("    mdisp: %d  pdisp: %d  vdisp: %d\n", mdisp, pdisp, vdisp);
}

struct TypeDescriptor {
	duint pTypeInfo;					// Always points to the type_info descriptor
	duint spare;
	char skip;						// Skip the period before the mangled name
	char sz_decorated_name[256];

	void Print();
};

inline void TypeDescriptor::Print()
{
	dprintf("TypeDescriptor\n");
	dprintf("  pTypeInfo: %p\n", pTypeInfo);
	dprintf("  sz_decorated_name: %s\n", sz_decorated_name);
}

struct RTTIBaseClassDescriptor
{
	DWORD pTypeDescriptor;				// type descriptor of the class
	DWORD numContainedBases;			// number of nested classes following in the Base Class Array
	struct PMD where;					// pointer-to-member displacement info
	DWORD attributes;					// flags, usually 0

	void Print(string name);
};

inline void RTTIBaseClassDescriptor::Print(string name)
{
	if (name == "")
		dprintf("  BaseClassDescriptor\n");
	else
		dprintf("  %s\n", name.c_str());

#ifdef _WIN64
	dprintf("    pTypeDescriptor: %p (Offset from module base)\n", pTypeDescriptor);
#else
	dprintf("    pTypeDescriptor: %p\n", pTypeDescriptor);
#endif

	dprintf("    numContainedBases: %d\n", numContainedBases);
	where.Print();
	dprintf("    attributes: %X\n", attributes);
}

// DWORD always
struct RTTIClassHierarchyDescriptor
{
	DWORD signature;								//always zero?
	DWORD attributes;								//bit 0 set = multiple inheritance, bit 1 set = virtual inheritance
	DWORD numBaseClasses;							//number of classes in pBaseClassArray
	DWORD pBaseClassArray;							// Index 0 of this array is always 'this' class first   in x64 this is an offset from the module base to the array

	void Print();
};

inline void RTTIClassHierarchyDescriptor::Print()
{
	dprintf("ClassHierarchyDescriptor\n");
	dprintf("  signature: %X attributes: %X\n", signature, attributes);

	string inheritanceMessage = "";
	if (CHECK_BIT(attributes, 0))
		inheritanceMessage += "Multiple Inheritance";
	if (CHECK_BIT(attributes, 1))
		inheritanceMessage += " | Virtual Inheritance";
	if (!inheritanceMessage.empty())
		dprintf("  Inheritance: %s\n", inheritanceMessage.c_str());

	dprintf("  numBaseClasses: %d\n", numBaseClasses);

#ifdef _WIN64
	dprintf("  pBaseClassArray: %p (Offset from module base)\n", pBaseClassArray);
#else
	dprintf("  pBaseClassArray: %p\n", pBaseClassArray);
#endif

}

struct RTTICompleteObjectLocator
{
	DWORD signature;								//always zero ?  (x64 always 1?)
	DWORD offset;									//offset of this vtable in the complete class
	DWORD cdOffset;									//constructor displacement offset
	DWORD pTypeDescriptor;							//TypeDescriptor of the complete class;   in x64 this field is an offset from the module base address
	DWORD pClassHierarchyDescriptor;				//describes inheritance hierarchy;		  in x64 this field is an offset from the module base address

	void Print();
};

inline void RTTICompleteObjectLocator::Print()
{
	dprintf("CompleteObjectLocator\n");
	dprintf("  signature: %08X offset: %08X cdOffset: %08X\n", signature, offset, cdOffset);
	dprintf("  pTypeDescriptor: %08X\n", pTypeDescriptor);
	dprintf("  pClassDescriptor: %08X\n", pClassHierarchyDescriptor);
}

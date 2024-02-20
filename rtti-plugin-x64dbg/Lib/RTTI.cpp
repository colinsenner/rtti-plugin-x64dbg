#include "Rtti.h"
#include "..\plugin.h"
#include "macros.h"
#include "MemHelpers.h"
#include "RTINFO.h"
#include <string>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

using namespace std;

string Demangle(char* sz_name)
{
	char tmp[MAX_CLASS_NAME] = { 0 };
	if (UnDecorateSymbolName(sz_name, tmp, MAX_CLASS_NAME, UNDNAME_NAME_ONLY) == 0)
		return false;

	// Remove 'AV' from the name
	char* n = tmp + 2;

	return string(n);
}

duint GetBaseAddress(duint addr)
{
	return DbgFunctions()->ModBaseFromAddr(addr);
}

RTTI::RTTI(duint addr)
{
	m_this = addr;

	m_isValid = GetRTTI();
}

duint RTTI::GetAddressVftable()
{
	duint vftable = 0;

	// Read the value at this to m_vftable
	if (!DbgMemRead(m_this, &vftable, sizeof(duint)))
		return 0;

	return vftable;
}

bool RTTI::GetVftable()
{
	duint vftable = GetAddressVftable();

	if (vftable == 0)
		return false;

	// Read offset at the vftable -4 is a pointer to a complete object locator
	duint pCompleteObjectLocator = (duint)SUBPTR(vftable, sizeof(duint));

	// Read the entire vftable
	if (!DbgMemRead(pCompleteObjectLocator, &m_vftable, sizeof(vftable_t)))
		return false;

	return true;
}

bool RTTI::GetCompleteObjectLocator()
{
	// Read the RTTICompleteObjectLocator
	if (!DbgMemRead((duint)m_vftable.pCompleteObjectLocator, &m_completeObjectLocator, sizeof(RTTICompleteObjectLocator)))
		return false;

	return true;
}

bool RTTI::GetTypeDescriptor()
{

#ifdef _WIN64
	duint moduleBase = GetBaseAddress((duint)m_vftable.pCompleteObjectLocator);
	duint pTypeDescriptor = (duint)ADDPTR(moduleBase, m_completeObjectLocator.pTypeDescriptor);
#else
	duint pTypeDescriptor = m_completeObjectLocator.pTypeDescriptor;
#endif

	// Read the TypeDescriptor
	if (!DbgMemRead(pTypeDescriptor, &m_typeDescriptor, sizeof(TypeDescriptor)))
		return false;

	return true;
}

bool RTTI::GetClassHierarchyDescriptor()
{
#ifdef _WIN64
	duint moduleBase = GetBaseAddress((duint)m_vftable.pCompleteObjectLocator);
	duint pClassHierarchyDescriptor = (duint)ADDPTR(moduleBase, m_completeObjectLocator.pClassHierarchyDescriptor);
#else
	duint pClassHierarchyDescriptor = m_completeObjectLocator.pClassHierarchyDescriptor;
#endif

	if (!DbgMemRead(pClassHierarchyDescriptor, &m_classHierarchyDescriptor, sizeof(RTTIClassHierarchyDescriptor)))
		return false;

	return true;
}

bool RTTI::GetBaseClasses()
{
#ifdef _WIN64
	duint addrBaseClassArray = (duint)ADDPTR(m_moduleBase, m_classHierarchyDescriptor.pBaseClassArray);
#else
	duint addrBaseClassArray = (duint)m_classHierarchyDescriptor.pBaseClassArray;
#endif

	duint addr = 0;
	duint numBaseClasses = m_classHierarchyDescriptor.numBaseClasses;

	if (numBaseClasses > MAX_BASE_CLASSES)
	{
		dprintf("Found %d base classes in the ClassHierarchyDescriptor, this seems unlikely, maximum we can save is %d.  Aborting.\n", m_classHierarchyDescriptor.numBaseClasses, MAX_BASE_CLASSES);
		return false;
	}

	// Populate the BaseClassArray
	// For each of the numBaseClasses populate the BaseClassDescriptors
	// Start at index 1, the first is always the base class, skip it
	for (size_t i = 1; i < numBaseClasses; i++)
	{
		addr = addrBaseClassArray + (i * sizeof(DWORD));
		DWORD val = 0;

		if (!DbgMemRead(addr, &val, sizeof(DWORD)))
			return false;

#ifdef _WIN64
		addr = (duint)ADDPTR(m_moduleBase, val);
#else
		addr = val;
#endif
		
		// Copy in the BaseClassDescriptor
		if (!DbgMemRead(addr, &m_baseClassDescriptors[i], sizeof(RTTIBaseClassDescriptor)))
			return false;

		auto baseClass = m_baseClassDescriptors[i];

#ifdef _WIN64
		addr = (duint)ADDPTR(m_moduleBase, m_baseClassDescriptors[i].pTypeDescriptor);
#else
		addr = m_baseClassDescriptors[i].pTypeDescriptor;
#endif

		// Copy in the BaseClassTypeDescriptor
		if (!DbgMemRead(addr, &m_baseClassTypeDescriptors[i], sizeof(TypeDescriptor)))
			return false;

		auto baseClassType = m_baseClassTypeDescriptors[i];

		string className = Demangle(baseClassType.sz_decorated_name);

		// Assign the vbtable entry
		m_vbtable[i] = 0;

		duint mdisp = baseClass.where.mdisp;
		duint pdisp = baseClass.where.pdisp;
		duint vdisp = baseClass.where.vdisp;

		//
		// Save each offset so we can display to the user where it is inside the class
		//

		m_baseClassOffsets[i] = (duint)ADDPTR(m_this, mdisp);

		if (baseClass.where.pdisp != -1)
		{
			// The docs aren't very clear here.
			// The pdisp field is the offset of the vbtable from this
			// Inside the vbtable we read at vdisp to get the final offset from the vbtable of the class
			m_vbtable[i] = (duint)ADDPTR(m_this, baseClass.where.pdisp);

			duint pMemberOffsets = 0;
			DWORD memberOffsets[MAX_BASE_CLASSES] = { 0 };

			// Read the value at the vdisp to find where the class is off the vbtable of this class
			//duint vbtable = 0;
			if (!DbgMemRead(m_vbtable[i], &pMemberOffsets, sizeof(pMemberOffsets)))
			{
				dprintf("Problem reading the vbtable.\n");
				continue;
			}

			if (!DbgMemRead(pMemberOffsets, &memberOffsets, sizeof(memberOffsets)))
			{
				dprintf("Problem reading the member offsets.\n");
				continue;
			}

			m_baseClassOffsets[i] = memberOffsets[i];
		}

		dprintf("\n");
		m_baseClassDescriptors[i].Print(className);
	}
	
	return true;
}

bool RTTI::GetRTTI()
{
	dprintf("=====================================================================================\n");

	// Parse vftable
	if (GetVftable() == false)
	{
		dprintf("Couldn't get the vftable.\n");
		return false;
	}
	duint vftable = GetAddressVftable();
	dprintf("vftable: %p\n", vftable);
	m_vftable.Print();

	// Parse CompleteObjectLocator
	if (GetCompleteObjectLocator() == false)
	{
		dprintf("Couldn't find the CompleteObjectLocator.\n");
		return false;
	}
	m_completeObjectLocator.Print();

	m_moduleBase = GetBaseAddress((duint)m_vftable.pCompleteObjectLocator);

	// Parse TypeDescriptor
	if (GetTypeDescriptor() == false)
	{
		dprintf("Couldn't parse the TypeDescriptor.\n");
		return false;
	}
	m_typeDescriptor.Print();

	// Demangle the name and copy it 
	name = Demangle(m_typeDescriptor.sz_decorated_name);

	// Read the ClassHierarchyDescriptor
	if (GetClassHierarchyDescriptor() == false)
	{
		dprintf("Couldn't parse the ClassHierarchyDescriptor.\n");
		return false;
	}
	m_classHierarchyDescriptor.Print();

	if (GetBaseClasses() == false)
	{
		dprintf("Couldn't parse base classes.\n");
		return false;
	}

	return true;
}

RTTIBaseClassDescriptor RTTI::GetBaseClassDescriptor(size_t idx)
{
	if (!m_isValid)
		return RTTIBaseClassDescriptor();

	if (idx > m_classHierarchyDescriptor.numBaseClasses)
		return RTTIBaseClassDescriptor();

	return m_baseClassDescriptors[idx];
}

string RTTI::ToString()
{
	string result = "";

	if (!m_isValid)
		return result;

	result = name.c_str();

	if (m_classHierarchyDescriptor.numBaseClasses > 1)
	{
		result.append("  :  ");

		// Base Class formatting
		// Appends each base class in this format
		// 'ClassA (+12), ClassB (+1C)'
		for (size_t i = 1; i < m_classHierarchyDescriptor.numBaseClasses; i++)
		{
			auto baseClass = GetBaseClassDescriptor(i);
			auto baseClassType = m_baseClassTypeDescriptors[i];
			auto baseClassName = Demangle(m_baseClassTypeDescriptors[i].sz_decorated_name);
			auto baseClassOffset = m_baseClassOffsets[i];

			result.append(baseClassName.c_str() + string(" "));

			// Print offsets
			result.append("(+");

			char hexStr[32] = { 0 };
			sprintf_s(hexStr, sizeof(hexStr), "%zX", GetBaseClassOffsetFromThis(i));
			
			result.append(hexStr);
			result.append(")");

			bool isLastClass = i == m_classHierarchyDescriptor.numBaseClasses - 1;
			if (!isLastClass)
				result.append(",");

			result.append(" ");
		}
	}

	return result;
}

duint RTTI::GetVbtable(size_t idx)
{
	if (idx > (sizeof(m_vbtable) / MAX_BASE_CLASSES))
		return 0;

	return m_vbtable[idx];
}

duint RTTI::GetBaseClassOffset(size_t idx)
{
	if (idx > (sizeof(m_baseClassOffsets) / MAX_BASE_CLASSES))
		return 0;

	return m_baseClassOffsets[idx];
}

duint RTTI::GetBaseClassOffsetFromThis(size_t idx)
{
	duint vbtable = GetVbtable(idx);
	duint offsetToThis = (duint)SUBPTR(vbtable, m_this);

	return (duint)ADDPTR(offsetToThis, GetBaseClassOffset(idx));
}

bool RTTI::IsValid()
{
	return m_isValid;
}
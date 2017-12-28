#include "class.h"
#include <filesystem.h>

CVMFClass::CVMFClass()
{
	m_szClassName[0] = '\0';
	m_properties = new CUtlVector<CVMFProperty>();
	m_subclasses = new CUtlVector<CVMFClass>();
}

CVMFClass::CVMFClass(const char * szClassname)
{
	V_strncpy(m_szClassName, szClassname, 64);
}

CVMFClass::~CVMFClass()
{
	delete m_properties;
	delete m_subclasses;
}

void CVMFClass::Write(FileHandle_t f)
{
	char buf[512];

	// Begin class
	V_snprintf(buf, 512, "%s\n{\n", m_szClassName);
	g_pFullFileSystem->Write(buf, V_strlen(buf), f);

	// Properties
	for (int i = 0; i < m_properties->Count(); i++)
	{
		m_properties->Element(i).Write(f);
	}

	// Sub-classes
	for (int i = 0; i < m_subclasses->Count(); i++)
	{
		m_subclasses->Element(i).Write(f);
	}
	// End class
	V_snprintf(buf, 512, "}\n");
	g_pFullFileSystem->Write(buf, V_strlen(buf), f);
}

void CVMFClass::AddClass(CVMFClass& c)
{
	m_subclasses->AddToTail(c);
}

void CVMFClass::AddProperty(CVMFProperty& p)
{
	m_properties->AddToTail(p);
}

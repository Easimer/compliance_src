#pragma once
#include "serializable.h"
#include "property.h"
#include <list.h>

class CVMFClass : public ISerializable {
public:
	CVMFClass();
	CVMFClass(const char* szClassname);
	~CVMFClass();
	void Write(FileHandle_t f);

	void AddProperty(CVMFProperty& p);
	void AddClass(CVMFClass& c);

	char m_szClassName[64];
	CUtlVector<CVMFProperty>* m_properties;
	CUtlVector<CVMFClass>* m_subclasses;

};
#pragma once

#include "serializable.h"
#include "class.h"

class CVMFRoot : ISerializable {
public:
	void Write(FileHandle_t* f);
	void AddClass(CVMFClass& c);
	void AddWorld(CVMFClass& c);
private:
	GList<CVMFClass> m_classes;
	Position m_iWorldPosition;
};
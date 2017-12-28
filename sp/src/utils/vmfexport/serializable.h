#pragma once

#include <filesystem.h>

class ISerializable {
public:
	virtual void Write(FileHandle_t f) = 0;
};
#define DECLARE_CLASS(className, baseClassName) \
typedef className ThisClass; \
typedef baseClassName BaseClass;
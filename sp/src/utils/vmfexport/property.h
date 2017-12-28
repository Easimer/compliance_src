#pragma once
#include "serializable.h"

typedef enum {
	PT_NONE, PT_INT, PT_DEC, PT_SZ, PT_BOOL, PT_VERTEX, PT_RGB, PT_PLANE, PT_UV, PT_DECARRAY
} prop_type_t;

typedef enum {
	NT_NORMAL, NT_BRACKET, NT_PARENTHESES
} prop_notation_t;

class CVMFProperty : public ISerializable {
public:
	CVMFProperty();
	CVMFProperty(const char* szKey, int nValue);
	CVMFProperty(const char* szKey, float flValue);
	CVMFProperty(const char* szKey, char* szValue);
	CVMFProperty(const char* szKey, bool bValue);
	CVMFProperty(const char* szKey, Vector& vVertex, prop_notation_t iNotation = NT_NORMAL);
	CVMFProperty(const char * szKey, Color& vColor);
	CVMFProperty(const char* szKey, Vector* pData);
	CVMFProperty(const char* szKey, float* pflData, int nCount, bool bIsUV = false);
	void Write(FileHandle_t f);

private:
	char m_szKey[128];
	prop_type_t m_iType;
	union {
		int integer;
		float decimal;
		char string[256];
		bool boolean;
		float vertex[3];
		float plane[9];
		float uv[5];
		float decarray[48]; // increase length if needed; 48 is the length of a normal prop of a dispinfo of a power 4 displacement
	} m_value;

	bool m_bIsUV;
	prop_notation_t m_iNotation;
	int m_nArrayLen;
};
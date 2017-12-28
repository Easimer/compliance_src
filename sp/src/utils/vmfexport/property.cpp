#include "property.h"

void CVMFProperty::Write(FileHandle_t f)
{
	char buf[1024];
	// write key first
	V_snprintf(buf, 1024, "\"%s\" \"", m_szKey);
	g_pFullFileSystem->Write(buf, V_strlen(buf), f);
	// value
	switch (m_iType)
	{
	case PT_INT:
		V_snprintf(buf, 1024, "%d", m_value.integer);
		break;
	case PT_DEC:
		V_snprintf(buf, 1024, "%f", m_value.decimal);
		break;
	case PT_SZ:
		V_snprintf(buf, 1024, "%s", m_value.string);
		break;
	case PT_BOOL:
		V_snprintf(buf, 1024, "%d", m_value.boolean ? 1 : 0);
		break;
	case PT_VERTEX:
		switch (m_iNotation)
		{
		case NT_NORMAL:
			V_snprintf(buf, 1024, "%f %f %f", m_value.vertex[0], m_value.vertex[1], m_value.vertex[2]);
			break;
		case NT_BRACKET:
			V_snprintf(buf, 1024, "[%f %f %f]", m_value.vertex[0], m_value.vertex[1], m_value.vertex[2]);
			break;
		case NT_PARENTHESES:
			V_snprintf(buf, 1024, "(%f %f %f)", m_value.vertex[0], m_value.vertex[1], m_value.vertex[2]);
			break;
		}
		break;
	case PT_RGB:
		/*V_snprintf(buf, 1024, "%f %f %f", m_value.[0], m_value.vertex[1], m_value.vertex[2]);
		break;*/
		buf[0] = '\0';
		break;
	case PT_PLANE:
		V_snprintf(buf, 1024, "({1} {2} {3}) ({4} {5} {6}) ({7} {8} {9})", m_value.plane[0], m_value.plane[1], m_value.plane[2], m_value.plane[3], m_value.plane[4], m_value.plane[5], m_value.plane[6], m_value.plane[7], m_value.plane[8]);
		break;
	case PT_UV:
		V_snprintf(buf, 1024, "[%f %f %f %f] %f", m_value.uv[0], m_value.uv[1], m_value.uv[2], m_value.uv[3], m_value.uv[4]);
		break;
	case PT_DECARRAY:
		char buf2[32];
		buf[0] = '\0';
		for (int i = 0; i < m_nArrayLen; i++)
		{
			V_snprintf(buf2, 32, "%f ", m_value.decarray[i]);
			V_strncat(buf, buf2, 1024);
		}
		break;
	}
	V_strncat(buf, "\"\n", 1024);
	g_pFullFileSystem->Write(buf, V_strlen(buf), f);
}

CVMFProperty::CVMFProperty()
{
	m_szKey[0] = '\0';
	m_iType = PT_NONE;
}

CVMFProperty::CVMFProperty(const char * szKey, int nValue)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_INT;
	m_value.integer = nValue;
}

CVMFProperty::CVMFProperty(const char * szKey, float flValue)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_DEC;
	m_value.decimal = flValue;
}

CVMFProperty::CVMFProperty(const char * szKey, char * szValue)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_SZ;
	V_strncpy(m_value.string, szValue, 256);
}

CVMFProperty::CVMFProperty(const char * szKey, bool bValue)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_BOOL;
	m_value.boolean = bValue;
}

CVMFProperty::CVMFProperty(const char * szKey, Vector & vVertex, prop_notation_t iNotation)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_VERTEX;
	m_value.vertex[0] = vVertex.x;
	m_value.vertex[1] = vVertex.y;
	m_value.vertex[2] = vVertex.z;
	m_iNotation = iNotation;
}

CVMFProperty::CVMFProperty(const char * szKey, Color& vColor)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_RGB;
	/*m_value.vertex[0] = vColor[0];
	m_value.vertex[1] = vColor.g;
	m_value.vertex[2] = vColor.b;*/
}

CVMFProperty::CVMFProperty(const char * szKey, Vector * pData)
{
	V_strncpy(m_szKey, szKey, 128);
	m_iType = PT_PLANE;
	for (int i = 0; i < 3; i++)
	{
		m_value.plane[i * 3 + 0] = pData[i * 3].x;
		m_value.plane[i * 3 + 1] = pData[i * 3].y;
		m_value.plane[i * 3 + 2] = pData[i * 3].z;
	}
}

CVMFProperty::CVMFProperty(const char * szKey, float * pflData, int nCount, bool bIsUV)
{
	V_strncpy(m_szKey, szKey, 128);
	if (bIsUV)
	{
		m_iType = PT_UV;
		V_memcpy(m_value.uv, pflData, 5 * sizeof(float));
	}
	else
	{
		m_iType = PT_DECARRAY;
		V_memcpy(m_value.decarray, pflData, nCount * sizeof(float));
		m_nArrayLen = nCount;
	}
}

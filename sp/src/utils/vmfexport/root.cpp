#include "root.h"

void CVMFRoot::Write(FileHandle_t* f)
{
	if (m_classes.IsEmpty())
		return;
	for (int i = 0; i < m_classes.GetNumItems(); i++)
	{
		m_classes.GetItemAtIndex(i).Write(f);
	}
}

void CVMFRoot::AddClass(CVMFClass & c)
{
	m_classes.InsertAtTail(c);
}

void CVMFRoot::AddWorld(CVMFClass & c)
{
	m_iWorldPosition = m_classes.InsertAtTail(c);
}

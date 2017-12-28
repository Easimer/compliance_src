#include <cbase.h>
#include <viewrender.h>
#include <view_scene.h>
#include <ScreenSpaceEffects.h>

#include "PostFX.h"

ConVar postfx_enable("postfx_enable", "1", 0, "Enables/disables ALL postfx");
ConVar postfx_demo("postfx_demo", "0", 0, "Apply post-fx only on right half of the screen");

CUtlLinkedList<postfx*>* postfx_list = NULL;

void PostFXInit()
{
	Warning("PostFXInit\n");
	postfx_list = new CUtlLinkedList<postfx*>();

	DECLARE_POSTFX("chroma");
}

void PostFXDeinit()
{
	Warning("PostFXDeinit\n");
	for (auto it = postfx_list->begin(); it != postfx_list->end(); ++it)
	{
		delete *it;
	}
	if (postfx_list)
		delete postfx_list;
}

void PostFXPerform(int x, int y, int width, int height)
{
	if (postfx_enable.GetInt() != 1)
		return;
	// early return if the hardware is shit
	if (g_pMaterialSystemHardwareConfig->GetDXSupportLevel() < 90)
	{
		return;
	}

	if (postfx_demo.GetInt() == 1)
	{
		width /= 2;
		x += width;
	}
	
	for (auto it = postfx_list->begin(); it != postfx_list->end(); ++it)
	{
		(*it)->apply(x, y, width, height);
	}
}

postfx::postfx(const char * szName)
{
	char szFilename[128] = { '\0' };
	Q_strncat(szFilename, "postfx/", 128);
	Q_strncat(szFilename, szName, 128);

	IMaterial* pOverlay = materials->FindMaterial(szFilename, TEXTURE_GROUP_CLIENT_EFFECTS, true);
	Assert(pOverlay && !IsErrorMaterial(pOverlay));
	m_pMaterial = pOverlay;
	m_pMaterial->IncrementReferenceCount();

	char szConvar[128] = { '\0' };
	Q_strncat(szConvar, "postfx_fx_", 128);
	Q_strncat(szConvar, szName, 128);

	//m_pConvar = new ConVar(szConvar, "1", 0, "Enable/disable this effect");

	Warning("PostFX \"%s\" created!\n", szName);
}

postfx::~postfx()
{
	if (m_pMaterial)
		m_pMaterial->DecrementReferenceCount();
	//if (m_pConvar)
	//	delete m_pConvar;
	Warning("PostFX cleanup!\n");
}

void postfx::apply(int x, int y, int w, int h)
{
	//if(m_pConvar->GetInt() != 0)
		DrawScreenEffectMaterial(m_pMaterial, x, y, w, h);
}

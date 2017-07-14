#include "cbase.h"
#include "eiface.h"
#include "eifacev21.h"
#include "gameinterface.h"
#include "memdbgon.h"

void TerrainMod_Add(TerrainModType type, const CTerrainModParams& params)
{
	((VEngineServerV21::IVEngineServer*)engine)->ApplyTerrainMod(type, params);
}
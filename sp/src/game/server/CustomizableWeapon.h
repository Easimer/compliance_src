#pragma once

#include "basehlcombatweapon.h"

class CCustomizableWeapon : public CBaseHLCombatWeapon {
public:
	DECLARE_CLASS(CCustomizableWeapon, CBaseHLCombatWeapon);

	CCustomizableWeapon();

	Vector GetCustomColor() { return m_vecColor; };
private:
	Vector m_vecColor;
};
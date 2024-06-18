#ifndef WEAPON_PARSE_TBE_H
#define WEAPON_PARSE_TBE_H

#include "weapon_parse.h"

class CTBEWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CTBEWeaponInfo, FileWeaponInfo_t );

	void Parse( KeyValues *pKeyValuesData, const char *szWeaponName ) OVERRIDE;

	float m_flFireRate;
	float m_flFireRateScoped;
	float m_flAccuracy;
	float m_flAccuracyAlt;
};

#endif // WEAPON_PARSE_TBE_H
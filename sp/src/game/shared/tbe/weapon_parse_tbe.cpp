#include "cbase.h"

#include <KeyValues.h>
#include "weapon_parse_tbe.h"

// membdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

FileWeaponInfo_t *CreateWeaponInfo()
{
	return new CTBEWeaponInfo;
}

void CTBEWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	m_flFireRate = pKeyValuesData->GetFloat( "firerate", 1.0f );
	m_flFireRateScoped = pKeyValuesData->GetFloat( "firerate_scoped", 1.0f );
	m_flAccuracy = pKeyValuesData->GetFloat( "accuracy", 0.0f );
	m_flAccuracyAlt = pKeyValuesData->GetFloat( "accuracy_alt", 0.0f );
}
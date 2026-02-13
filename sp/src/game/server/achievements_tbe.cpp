//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================


#include "cbase.h"

#ifdef GAME_DLL

#include "achievementmgr.h"
#include "baseachievement.h"
#include "iservervehicle.h"
#include "hl2_shareddefs.h"

// Where'd He Go - Defeat Fabreen in the 20s
class CAchievementFabreenNoir : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_FABREEN_NOIR"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_FABREEN_NOIR");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementFabreenNoir, ACHIEVEMENT_FABREEN_NOIR, "ACH_FABREEN_NOIR", 5);

// Actually Pretty Capable - Blow up the 40s APC
class CAchievementAPC40s : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_APC_40S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_APC_40S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementAPC40s, ACHIEVEMENT_APC_40S, "ACH_APC_40S", 5);

// This Bird Is Baked - Blow up the 80s chopper
class CAchievementHelicopter80S : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_HELICOPTER_80S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_HELICOPTER_80S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementHelicopter80S, ACHIEVEMENT_HELICOPTER_80S, "ACH_HELICOPTER_80S", 5);

// This Time, They Have Legs - Blow up the two striders
class CAchievementStriders2010S : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_STRIDERS_2010S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_STRIDERS_2010S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementStriders2010S, ACHIEVEMENT_STRIDERS_2010S, "ACH_STRIDERS_2010S", 5);

// We Could've Been Contenders - Finish off Terminus Fabreen
class CAchievementFabreenTerminus : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_FABREEN_TERMINUS"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_FABREEN_TERMINUS");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementFabreenTerminus, ACHIEVEMENT_FABREEN_TERMINUS, "ACH_FABREEN_TERMINUS", 5);

// Maybe The World Ain't So Noir... - Beat the 20s
class CAchievementBeat20S : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_BEAT_20S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_BEAT_20S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementBeat20S, ACHIEVEMENT_BEAT_20S, "ACH_BEAT_20S", 5);

// What Do You Mean, "Second World War?" - Beat the 40s
class CAchievementBeat40S : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_BEAT_40S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_BEAT_40S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementBeat40S, ACHIEVEMENT_BEAT_40S, "ACH_BEAT_40S", 5);

// Hotline Zachary - Beat the 80s
class CAchievementBeat80S : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_BEAT_80S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_BEAT_80S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementBeat80S, ACHIEVEMENT_BEAT_80S, "ACH_BEAT_80S", 5);

// Into The Burtonverse - Beat the 2010s
class CAchievementBeat2010S : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_BEAT_2010S"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_BEAT_2010S");
		SetGameDirFilter("theburtonequation");
		SetGoal(1);
	}
};
DECLARE_ACHIEVEMENT(CAchievementBeat2010S, ACHIEVEMENT_BEAT_2010S, "ACH_BEAT_2010S", 5);

// What Does The "G" Stand For Anyway? - Find all G-Woman sightings	
class CAchievementFindGWoman : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_GWOMAN_RG01", "TBE_GWOMAN_RG02", "TBE_GWOMAN_RG03", "TBE_GWOMAN_RG04"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_GWOMAN");
		SetGameDirFilter("theburtonequation");
		SetGoal(m_iNumComponents);
	}

	// Show progress for this achievement
	virtual bool ShouldShowProgressNotification() { return true; }
};
DECLARE_ACHIEVEMENT(CAchievementFindGWoman, ACHIEVEMENT_FIND_GWOMAN, "ACH_FIND_GWOMAN", 5);

// Gotta Go Back, Back To The... - Find all the Beloryanns
class CAchievementFindBeloyranns : public CBaseAchievement
{
	virtual void Init()
	{
		static const char* szComponents[] =
		{
			"TBE_BELORYANN_RG01", "TBE_BELORYANN_RG02", "TBE_BELORYANN_RG03", "TBE_BELORYANN_RG04"
		};
		SetFlags(ACH_HAS_COMPONENTS | ACH_LISTEN_COMPONENT_EVENTS | ACH_SAVE_GLOBAL);
		m_pszComponentNames = szComponents;
		m_iNumComponents = ARRAYSIZE(szComponents);
		SetComponentPrefix("TBE_BELORYANN");
		SetGameDirFilter("theburtonequation");
		SetGoal(m_iNumComponents);
	}

	// Show progress for this achievement
	virtual bool ShouldShowProgressNotification() { return true; }
};
DECLARE_ACHIEVEMENT(CAchievementFindBeloyranns, ACHIEVEMENT_FIND_BELORYANNS, "ACH_FIND_BELORYANNS", 5);

#endif // GAME_DLL

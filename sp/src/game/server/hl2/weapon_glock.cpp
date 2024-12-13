//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Pistol - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "vstdlib/random.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	PISTOL_FASTEST_REFIRE_TIME		0.1f
#define	PISTOL_FASTEST_DRY_REFIRE_TIME	0.2f

#define	PISTOL_ACCURACY_SHOT_PENALTY_TIME		0.2f	// Applied amount of time each shot adds to the time we must recover from
#define	PISTOL_ACCURACY_MAXIMUM_PENALTY_TIME	1.5f	// Maximum penalty to deal out

ConVar	glock_use_new_accuracy( "glock_use_new_accuracy", "1" );

ConVar	sk_plr_dmg_glock		( "sk_plr_dmg_glock", "0", FCVAR_REPLICATED );
ConVar	sk_npc_dmg_glock		( "sk_npc_dmg_glock", "0", FCVAR_REPLICATED );
ConVar  sk_glock_burst_time( "sk_glock_burst_time", "0.25", 0 );
ConVar  sk_glock_burst_size( "sk_glock_burst_size", "3", 0 );
ConVar  sk_glock_burst_speed( "sk_glock_burst_speed", "3", 0 );

//-----------------------------------------------------------------------------
// CWeaponGLOCK
//-----------------------------------------------------------------------------

class CWeaponGLOCK : public CHLSelectFireMachineGun
{
	DECLARE_DATADESC();

public:
	DECLARE_CLASS( CWeaponGLOCK, CHLSelectFireMachineGun );

	CWeaponGLOCK(void);

	DECLARE_SERVERCLASS();

	void	Spawn( void );
	void	Precache( void );
	void	ItemPostFrame( void );
	void	ItemPreFrame( void );
	void	ItemBusyFrame( void );
	void	PrimaryAttack( void );
	void	SecondaryAttack( void );
	void	AddViewKick( void );
	virtual float	GetBurstCycleRate( void ) { return sk_glock_burst_time.GetFloat(); };
	virtual int GetBurstSize( void ) { return sk_glock_burst_size.GetInt(); };
	void	DryFire( void );
	void	Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

	void	UpdatePenaltyTime( void );

	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	Activity	GetPrimaryAttackActivity( void );
	
	virtual bool Reload( void );

	virtual const Vector& GetBulletSpread( void )
	{		
		// Handle NPCs first
		static Vector npcCone = VECTOR_CONE_5DEGREES;
		if ( GetOwner() && GetOwner()->IsNPC() )
			return npcCone;
			
		static Vector cone;
		if (m_iBurstSize == 0)
		{
			if (glock_use_new_accuracy.GetBool())
			{
				float ramp = RemapValClamped( m_flAccuracyPenalty,
					0.0f,
					PISTOL_ACCURACY_MAXIMUM_PENALTY_TIME,
					0.0f,
					1.0f );

				// We lerp from very accurate to inaccurate over time
				VectorLerp( m_vecBulletSpread, m_vecBulletSpread * 6, ramp, cone );
			}
			else
			{
				// Old value
				cone = m_vecBulletSpread * 4;
			}
		}
		else {
			cone = m_vecBulletSpreadAlt;
		}

		return cone;
	}
	
	virtual int	GetMinBurst() 
	{ 
		return 1; 
	}

	virtual int	GetMaxBurst() 
	{ 
		return 3; 
	}

	virtual float GetFireRate( void )
	{
		if (m_iBurstSize == 0)
			return GetTBEWpnData().m_flFireRate;
		else
			return GetTBEWpnData().m_flFireRate / sk_glock_burst_speed.GetFloat();
	}

	// Added by 1upD - damage override methods for when you need damage to not be the ammo type's damage
	virtual float GetDamageOverride();
	virtual float GetPlayerDamageOverride();

	DECLARE_ACTTABLE();

private:
	float	m_flSoonestPrimaryAttack;
	float	m_flLastAttackTime;
	float	m_flAccuracyPenalty;
	int		m_nNumShotsFired;
	Vector	m_vecBulletSpread = vec3_invalid;
	Vector	m_vecBulletSpreadAlt = vec3_invalid;
};


IMPLEMENT_SERVERCLASS_ST(CWeaponGLOCK, DT_WeaponGLOCK)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_glock, CWeaponGLOCK );
PRECACHE_WEAPON_REGISTER( weapon_glock );

BEGIN_DATADESC( CWeaponGLOCK )

	DEFINE_FIELD( m_flSoonestPrimaryAttack, FIELD_TIME ),
	DEFINE_FIELD( m_flLastAttackTime,		FIELD_TIME ),
	DEFINE_FIELD( m_flAccuracyPenalty,		FIELD_FLOAT ), //NOTENOTE: This is NOT tracking game time
	DEFINE_FIELD( m_nNumShotsFired,			FIELD_INTEGER ),
	DEFINE_FIELD( m_vecBulletSpread,		FIELD_VECTOR ),
	DEFINE_FIELD( m_vecBulletSpreadAlt,		FIELD_VECTOR ),

END_DATADESC()

acttable_t	CWeaponGLOCK::m_acttable[] = 
{
	{ ACT_IDLE,						ACT_IDLE_PISTOL,				true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_PISTOL,			true },
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_PISTOL,		true },
	{ ACT_RELOAD,					ACT_RELOAD_PISTOL,				true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_PISTOL,			true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_PISTOL,				true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_PISTOL,true },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_PISTOL_LOW,			false },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_PISTOL_LOW,	false },
	{ ACT_COVER_LOW,				ACT_COVER_PISTOL_LOW,			false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_PISTOL_LOW,		false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_WALK,						ACT_WALK_PISTOL,				false },
	{ ACT_RUN,						ACT_RUN_PISTOL,					false },
};


IMPLEMENT_ACTTABLE( CWeaponGLOCK );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponGLOCK::CWeaponGLOCK( void )
{
	m_flSoonestPrimaryAttack = gpGlobals->curtime;
	m_flAccuracyPenalty = 0.0f;

	m_fMinRange1		= 24;
	m_fMaxRange1		= 1500;
	m_fMinRange2		= 24;
	m_fMaxRange2		= 200;

	m_bFiresUnderwater	= true;
}

void CWeaponGLOCK::Spawn( void )
{
	BaseClass::Spawn();
	SETUP_WEAPON_ACCURACY();
	SETUP_WEAPON_ALT_ACCURACY();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGLOCK::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeaponGLOCK::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	switch( pEvent->event )
	{
		case EVENT_WEAPON_PISTOL_FIRE:
		{
			Vector vecShootOrigin, vecShootDir;
			vecShootOrigin = pOperator->Weapon_ShootPosition();

			CAI_BaseNPC *npc = pOperator->MyNPCPointer();
			ASSERT( npc != NULL );

			vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );

			CSoundEnt::InsertSound( SOUND_COMBAT|SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_PISTOL, 0.2, pOperator, SOUNDENT_CHANNEL_WEAPON, pOperator->GetEnemy() );

			WeaponSound( SINGLE_NPC );
			// 1upD - make sure to add parameters to pOperator->FireBullets to include override damage from the weapon. The two parameters before that are no longer used, they can be 0
			pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1, 0, 0, GetDamageOverride() );
			pOperator->DoMuzzleFlash();
			m_iClip1 = m_iClip1 - 1;
		}
		break;
		default:
			BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponGLOCK::DryFire( void )
{
	WeaponSound( EMPTY );
	SendWeaponAnim( ACT_VM_DRYFIRE );
	
	m_flSoonestPrimaryAttack	= gpGlobals->curtime + PISTOL_FASTEST_DRY_REFIRE_TIME;
	m_flNextPrimaryAttack		= gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponGLOCK::PrimaryAttack( void )
{
	// suppress weird m1+m2 stuff
	if (m_iBurstSize > 0)
		return;

	if ( ( gpGlobals->curtime - m_flLastAttackTime ) > 0.5f )
	{
		m_nNumShotsFired = 0;
	}
	else
	{
		m_nNumShotsFired++;
	}

	m_flLastAttackTime = gpGlobals->curtime;
	m_flSoonestPrimaryAttack = gpGlobals->curtime + PISTOL_FASTEST_REFIRE_TIME;
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_PISTOL, 0.2, GetOwner() );

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if( pOwner )
	{
		// Each time the player fires the pistol, reset the view punch. This prevents
		// the aim from 'drifting off' when the player fires very quickly. This may
		// not be the ideal way to achieve this, but it's cheap and it works, which is
		// great for a feature we're evaluating. (sjb)
		pOwner->ViewPunchReset();
	}

	BaseClass::PrimaryAttack();

	// Add an accuracy penalty which can move past our maximum penalty time if we're really spastic
	m_flAccuracyPenalty += PISTOL_ACCURACY_SHOT_PENALTY_TIME;

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pOwner, true, GetClassname() );

	m_flNextSecondaryAttack = gpGlobals->curtime + GetFireRate();
}

void CWeaponGLOCK::SecondaryAttack() {
	// suppress weird m1+m2 stuff
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if (pOwner)
	{
		if (pOwner->m_nButtons & IN_ATTACK) {
			return;
		}
	}
	if (Clip1() == 1)
	{
		// no point doing the double burst when we can't even fire 2 bullets
		PrimaryAttack();
		return;
	}

	m_iBurstSize = GetBurstSize();

	// Call the think function directly so that the first round gets fired immediately.
	BurstSingleSoundThink();
	SetThink( &CHLSelectFireMachineGun::BurstSingleSoundThink );
	m_flNextPrimaryAttack = gpGlobals->curtime + GetBurstCycleRate();
	m_flNextSecondaryAttack = gpGlobals->curtime + GetBurstCycleRate();

	// Pick up the rest of the burst through the think function.
	SetNextThink( gpGlobals->curtime + GetFireRate() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGLOCK::UpdatePenaltyTime( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Check our penalty time decay
	if ( ( ( pOwner->m_nButtons & IN_ATTACK ) == false ) && ( m_flSoonestPrimaryAttack < gpGlobals->curtime ) )
	{
		m_flAccuracyPenalty -= gpGlobals->frametime;
		m_flAccuracyPenalty = clamp( m_flAccuracyPenalty, 0.0f, PISTOL_ACCURACY_MAXIMUM_PENALTY_TIME );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGLOCK::ItemPreFrame( void )
{
	UpdatePenaltyTime();

	BaseClass::ItemPreFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGLOCK::ItemBusyFrame( void )
{
	UpdatePenaltyTime();

	BaseClass::ItemBusyFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Allows firing as fast as button is pressed
//-----------------------------------------------------------------------------
void CWeaponGLOCK::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();

	if ( m_bInReload )
		return;
	
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	//Allow a refire as fast as the player can click
	if ( ( ( pOwner->m_nButtons & IN_ATTACK ) == false ) && ( m_flSoonestPrimaryAttack < gpGlobals->curtime ) )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime - 0.1f;
	}
	else if ( ( pOwner->m_nButtons & IN_ATTACK ) && ( m_flNextPrimaryAttack < gpGlobals->curtime ) && ( m_iClip1 <= 0 ) )
	{
		DryFire();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
Activity CWeaponGLOCK::GetPrimaryAttackActivity( void )
{
	if ( m_nNumShotsFired < 1 )
		return ACT_VM_PRIMARYATTACK;

	if ( m_nNumShotsFired < 2 )
		return ACT_VM_RECOIL1;

	if ( m_nNumShotsFired < 3 )
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponGLOCK::Reload( void )
{
	// stop any existing bursts
	if (m_iBurstSize > 0)
	{
		// The burst is over!
		SetThink( NULL );
		// idle immediately to stop the firing animation
		SetWeaponIdleTime( gpGlobals->curtime );
		// Force set the burst size so we can use primary fire again
		m_iBurstSize = 0;
	}

	bool fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );
	if ( fRet )
	{
		WeaponSound( RELOAD );
		m_flAccuracyPenalty = 0.0f;
	}
	return fRet;
}

float CWeaponGLOCK::GetDamageOverride()
{
	return sk_npc_dmg_glock.GetFloat();
}

float CWeaponGLOCK::GetPlayerDamageOverride()
{
	return sk_plr_dmg_glock.GetFloat();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGLOCK::AddViewKick( void )
{
	CBasePlayer *pPlayer  = ToBasePlayer( GetOwner() );
	
	if ( pPlayer == NULL )
		return;

	QAngle	viewPunch;

	viewPunch.x = random->RandomFloat( 0.25f, 0.5f );
	viewPunch.y = random->RandomFloat( -.6f, .6f );
	viewPunch.z = 0.0f;

	//Add it to the view punch
	pPlayer->ViewPunch( viewPunch );
}

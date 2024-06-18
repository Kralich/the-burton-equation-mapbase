#include "cbase.h"
#include "basecombatweapon.h"
#include "player.h"
#include "weapon_ar2.h"
#include "grenade_ar2.h"
#include "in_buttons.h"
#include "rumble_shared.h"
#include "gamestats.h"
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "ai_basenpc.h"
#include "npcevent.h"

extern ConVar    sk_plr_dmg_smg1_grenade;

//-----------------------------------------------------------------------------
// OICW
//-----------------------------------------------------------------------------
class CWeaponOICW : public CHLSelectFireMachineGun
{
public:
	DECLARE_CLASS( CWeaponOICW, CHLSelectFireMachineGun );
	DECLARE_SERVERCLASS();

	CWeaponOICW();

	virtual void	ItemPostFrame( void );
	virtual void	Precache( void );

	void			SecondaryAttack( void );
	bool			Holster( CBaseCombatWeapon * pSwitchingTo );
	virtual void	Equip( CBaseCombatCharacter *pOwner );
	virtual bool	Reload();

	const char		*GetTracerType( void ) { return "AR2Tracer"; }

	void			AddViewKick( void );

	void			FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, bool bUseWeaponAngles );
	void			Operator_ForceNPCFire( CBaseCombatCharacter  *pOperator, bool bSecondary );
	void			Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

	void			ToggleScope( void );
	void			Scope( void );
	void			Unscope( void );

	int		GetMinBurst( void ) { return 2; }
	int		GetMaxBurst( void ) { return 5; }
	float	GetFireRate( void );

	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	Activity	GetPrimaryAttackActivity( void );

	void	DoImpactEffect( trace_t &tr, int nDamageType );

	static Vector m_vecBulletSpread;
	static Vector m_vecBulletSpreadAlt;
	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = m_vecBulletSpread;
		static Vector coneScoped;
		if (m_bIsScoped)
			coneScoped = m_vecBulletSpreadAlt;
		return m_bIsScoped ? coneScoped : cone;
	}

	const WeaponProficiencyInfo_t *GetProficiencyValues();

	CNetworkVar( bool, m_bIsScoped );

	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
private:
	float	m_flScopeTime;
};

BEGIN_DATADESC(CWeaponOICW)
	DEFINE_FIELD( m_flScopeTime, FIELD_TIME ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CWeaponOICW, DT_WeaponOICW)
	SendPropBool(SENDINFO(m_bIsScoped)),
END_SEND_TABLE()

// OICW variant of the AR2 for the Burton Equation
LINK_ENTITY_TO_CLASS(weapon_oicw, CWeaponOICW);
PRECACHE_WEAPON_REGISTER(weapon_oicw);

acttable_t	CWeaponOICW::m_acttable[] =
{
#if AR2_ACTIVITY_FIX == 1
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_AR2, true },
	{ ACT_RELOAD, ACT_RELOAD_AR2, true },
	{ ACT_IDLE, ACT_IDLE_AR2, true },
	{ ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_AR2, false },

	{ ACT_WALK, ACT_WALK_AR2, true },

	// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED, ACT_IDLE_AR2_RELAXED, false },//never aims
	{ ACT_IDLE_STIMULATED, ACT_IDLE_AR2_STIMULATED, false },
	{ ACT_IDLE_AGITATED, ACT_IDLE_ANGRY_AR2, false },//always aims

	{ ACT_WALK_RELAXED, ACT_WALK_AR2_RELAXED, false },//never aims
	{ ACT_WALK_STIMULATED, ACT_WALK_AR2_STIMULATED, false },
	{ ACT_WALK_AGITATED, ACT_WALK_AIM_AR2, false },//always aims

	{ ACT_RUN_RELAXED, ACT_RUN_AR2_RELAXED, false },//never aims
	{ ACT_RUN_STIMULATED, ACT_RUN_AR2_STIMULATED, false },
	{ ACT_RUN_AGITATED, ACT_RUN_AIM_RIFLE, false },//always aims

	// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED, ACT_IDLE_AR2_RELAXED, false },//never aims	
	{ ACT_IDLE_AIM_STIMULATED, ACT_IDLE_AIM_AR2_STIMULATED, false },
	{ ACT_IDLE_AIM_AGITATED, ACT_IDLE_ANGRY_AR2, false },//always aims

	{ ACT_WALK_AIM_RELAXED, ACT_WALK_AR2_RELAXED, false },//never aims
	{ ACT_WALK_AIM_STIMULATED, ACT_WALK_AIM_AR2_STIMULATED, false },
	{ ACT_WALK_AIM_AGITATED, ACT_WALK_AIM_AR2, false },//always aims

	{ ACT_RUN_AIM_RELAXED, ACT_RUN_AR2_RELAXED, false },//never aims
	{ ACT_RUN_AIM_STIMULATED, ACT_RUN_AIM_AR2_STIMULATED, false },
	{ ACT_RUN_AIM_AGITATED, ACT_RUN_AIM_RIFLE, false },//always aims
	//End readiness activities

	{ ACT_WALK_AIM, ACT_WALK_AIM_AR2, true },
	{ ACT_WALK_CROUCH, ACT_WALK_CROUCH_RIFLE, true },
	{ ACT_WALK_CROUCH_AIM, ACT_WALK_CROUCH_AIM_RIFLE, true },
	{ ACT_RUN, ACT_RUN_AR2, true },
	{ ACT_RUN_AIM, ACT_RUN_AIM_AR2, true },
	{ ACT_RUN_CROUCH, ACT_RUN_CROUCH_RIFLE, true },
	{ ACT_RUN_CROUCH_AIM, ACT_RUN_CROUCH_AIM_RIFLE, true },
	{ ACT_GESTURE_RANGE_ATTACK1, ACT_GESTURE_RANGE_ATTACK_AR2, false },
	{ ACT_COVER_LOW, ACT_COVER_AR2_LOW, true },
	{ ACT_RANGE_AIM_LOW, ACT_RANGE_AIM_AR2_LOW, false },
	{ ACT_RANGE_ATTACK1_LOW, ACT_RANGE_ATTACK_AR2_LOW, false },
	{ ACT_RELOAD_LOW, ACT_RELOAD_AR2_LOW, false },
	{ ACT_GESTURE_RELOAD, ACT_GESTURE_RELOAD_AR2, true },
	//	{ ACT_RANGE_ATTACK2, ACT_RANGE_ATTACK_AR2_GRENADE, true },
#else
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_AR2, true },
	{ ACT_RELOAD, ACT_RELOAD_SMG1, true },		// FIXME: hook to AR2 unique
	{ ACT_IDLE, ACT_IDLE_SMG1, true },		// FIXME: hook to AR2 unique
	{ ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_SMG1, true },		// FIXME: hook to AR2 unique

	{ ACT_WALK, ACT_WALK_RIFLE, true },

	// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED, ACT_IDLE_SMG1_RELAXED, false },//never aims
	{ ACT_IDLE_STIMULATED, ACT_IDLE_SMG1_STIMULATED, false },
	{ ACT_IDLE_AGITATED, ACT_IDLE_ANGRY_SMG1, false },//always aims

	{ ACT_WALK_RELAXED, ACT_WALK_RIFLE_RELAXED, false },//never aims
	{ ACT_WALK_STIMULATED, ACT_WALK_RIFLE_STIMULATED, false },
	{ ACT_WALK_AGITATED, ACT_WALK_AIM_RIFLE, false },//always aims

	{ ACT_RUN_RELAXED, ACT_RUN_RIFLE_RELAXED, false },//never aims
	{ ACT_RUN_STIMULATED, ACT_RUN_RIFLE_STIMULATED, false },
	{ ACT_RUN_AGITATED, ACT_RUN_AIM_RIFLE, false },//always aims

	// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED, ACT_IDLE_SMG1_RELAXED, false },//never aims	
	{ ACT_IDLE_AIM_STIMULATED, ACT_IDLE_AIM_RIFLE_STIMULATED, false },
	{ ACT_IDLE_AIM_AGITATED, ACT_IDLE_ANGRY_SMG1, false },//always aims

	{ ACT_WALK_AIM_RELAXED, ACT_WALK_RIFLE_RELAXED, false },//never aims
	{ ACT_WALK_AIM_STIMULATED, ACT_WALK_AIM_RIFLE_STIMULATED, false },
	{ ACT_WALK_AIM_AGITATED, ACT_WALK_AIM_RIFLE, false },//always aims

	{ ACT_RUN_AIM_RELAXED, ACT_RUN_RIFLE_RELAXED, false },//never aims
	{ ACT_RUN_AIM_STIMULATED, ACT_RUN_AIM_RIFLE_STIMULATED, false },
	{ ACT_RUN_AIM_AGITATED, ACT_RUN_AIM_RIFLE, false },//always aims
	//End readiness activities

	{ ACT_WALK_AIM, ACT_WALK_AIM_RIFLE, true },
	{ ACT_WALK_CROUCH, ACT_WALK_CROUCH_RIFLE, true },
	{ ACT_WALK_CROUCH_AIM, ACT_WALK_CROUCH_AIM_RIFLE, true },
	{ ACT_RUN, ACT_RUN_RIFLE, true },
	{ ACT_RUN_AIM, ACT_RUN_AIM_RIFLE, true },
	{ ACT_RUN_CROUCH, ACT_RUN_CROUCH_RIFLE, true },
	{ ACT_RUN_CROUCH_AIM, ACT_RUN_CROUCH_AIM_RIFLE, true },
	{ ACT_GESTURE_RANGE_ATTACK1, ACT_GESTURE_RANGE_ATTACK_AR2, false },
	{ ACT_COVER_LOW, ACT_COVER_SMG1_LOW, false },		// FIXME: hook to AR2 unique
	{ ACT_RANGE_AIM_LOW, ACT_RANGE_AIM_AR2_LOW, false },
	{ ACT_RANGE_ATTACK1_LOW, ACT_RANGE_ATTACK_SMG1_LOW, true },		// FIXME: hook to AR2 unique
	{ ACT_RELOAD_LOW, ACT_RELOAD_SMG1_LOW, false },
	{ ACT_GESTURE_RELOAD, ACT_GESTURE_RELOAD_SMG1, true },
	//	{ ACT_RANGE_ATTACK2, ACT_RANGE_ATTACK_AR2_GRENADE, true },
#endif

#if EXPANDED_HL2_WEAPON_ACTIVITIES
	{ ACT_ARM, ACT_ARM_RIFLE, false },
	{ ACT_DISARM, ACT_DISARM_RIFLE, false },
#endif

#if EXPANDED_HL2_COVER_ACTIVITIES
	{ ACT_RANGE_AIM_MED, ACT_RANGE_AIM_AR2_MED, false },
	{ ACT_RANGE_ATTACK1_MED, ACT_RANGE_ATTACK_AR2_MED, false },

	{ ACT_COVER_WALL_R, ACT_COVER_WALL_R_RIFLE, false },
	{ ACT_COVER_WALL_L, ACT_COVER_WALL_L_RIFLE, false },
	{ ACT_COVER_WALL_LOW_R, ACT_COVER_WALL_LOW_R_RIFLE, false },
	{ ACT_COVER_WALL_LOW_L, ACT_COVER_WALL_LOW_L_RIFLE, false },
#endif

#ifdef MAPBASE
	// HL2:DM activities (for third-person animations in SP)
	{ ACT_HL2MP_IDLE, ACT_HL2MP_IDLE_AR2, false },
	{ ACT_HL2MP_RUN, ACT_HL2MP_RUN_AR2, false },
	{ ACT_HL2MP_IDLE_CROUCH, ACT_HL2MP_IDLE_CROUCH_AR2, false },
	{ ACT_HL2MP_WALK_CROUCH, ACT_HL2MP_WALK_CROUCH_AR2, false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK, ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2, false },
	{ ACT_HL2MP_GESTURE_RELOAD, ACT_HL2MP_GESTURE_RELOAD_AR2, false },
	{ ACT_HL2MP_JUMP, ACT_HL2MP_JUMP_AR2, false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_HL2MP_WALK, ACT_HL2MP_WALK_AR2, false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK2, ACT_HL2MP_GESTURE_RANGE_ATTACK2_AR2, false },
#endif
#endif
};

IMPLEMENT_ACTTABLE( CWeaponOICW );

#ifdef MAPBASE
// Allows Weapon_BackupActivity() to access the AR2's activity table.
acttable_t *GetOICWActtable()
{
	return CWeaponOICW::m_acttable;
}

int GetOICWActtableCount()
{
	return ARRAYSIZE( CWeaponOICW::m_acttable );
}
#endif

CWeaponOICW::CWeaponOICW()
{
	m_fMinRange1 = 65;
	m_fMaxRange1 = 2048;

	m_fMinRange2 = 256;
	m_fMaxRange2 = 1024;

	m_nShotsFired = 0;

	m_bAltFiresUnderwater = false;
}

//-----------------------------------------------------------------------------
// Purpose: Precaches weapon
//-----------------------------------------------------------------------------
void CWeaponOICW::Precache( void )
{
	BaseClass::Precache();
	UTIL_PrecacheOther( "grenade_ar2" );
}

//-----------------------------------------------------------------------------
// Purpose: Deals with scope
//-----------------------------------------------------------------------------
void CWeaponOICW::ItemPostFrame( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (GetActivity() != ACT_VM_DRAW) {
		if (pPlayer->m_nButtons & IN_ATTACK3)
		{
			if (m_flScopeTime <= gpGlobals->curtime)
			{
				ToggleScope();
			}
		}
	}

	BaseClass::ItemPostFrame();

}

Vector CWeaponOICW::m_vecBulletSpread;
Vector CWeaponOICW::m_vecBulletSpreadAlt;

void CWeaponOICW::Equip( CBaseCombatCharacter *pOwner )
{
	// init accuracy and cache it, to avoid doing unnecessary trig
	float flCone = sinf( GetTBEWpnData().m_flAccuracy * M_PI / 360 ); // perform the same calculations as are used to find VECTOR_CONE_<X>DEGREES
	m_vecBulletSpread = Vector( flCone, flCone, flCone );

	float flConeAlt = sinf( GetTBEWpnData().m_flAccuracyAlt * M_PI / 360 ); // perform the same calculations as are used to find VECTOR_CONE_<X>DEGREES
	m_vecBulletSpreadAlt = Vector( flConeAlt, flConeAlt, flConeAlt );

	BaseClass::Equip( pOwner );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponOICW::GetPrimaryAttackActivity( void )
{
	if (m_nShotsFired < 2)
		return ACT_VM_PRIMARYATTACK;

	if (m_nShotsFired < 3)
		return ACT_VM_RECOIL1;

	if (m_nShotsFired < 4)
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &tr - 
//			nDamageType - 
//-----------------------------------------------------------------------------
void CWeaponOICW::DoImpactEffect( trace_t &tr, int nDamageType )
{
	CEffectData data;

	data.m_vOrigin = tr.endpos + (tr.plane.normal * 1.0f);
	data.m_vNormal = tr.plane.normal;

	DispatchEffect( "AR2Impact", data );

	BaseClass::DoImpactEffect( tr, nDamageType );
}

//-----------------------------------------------------------------------------
// Purpose: Descope weapon on holster
//-----------------------------------------------------------------------------
bool CWeaponOICW::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	if (GetOwner()->IsPlayer()) {
		if (m_bIsScoped)
		{
			Unscope();
		}
	}

	return BaseClass::Holster( pSwitchingTo );
}

float CWeaponOICW::GetFireRate()
{
	if (!GetOwner()->IsNPC()) {
		if (m_bIsScoped) {
			return GetTBEWpnData().m_flFireRateScoped;
		}
		else {
			return GetTBEWpnData().m_flFireRate;
		}
	}
	else {
		return GetTBEWpnData().m_flFireRate;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Fire grenade
//-----------------------------------------------------------------------------
void CWeaponOICW::SecondaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (pPlayer == NULL)
		return;

	if (m_bIsScoped)
		return;

	//Must have ammo
	if ((pPlayer->GetWaterLevel() == 3))
	{
		SendWeaponAnim( ACT_VM_DRYFIRE );
		BaseClass::WeaponSound( EMPTY );
		m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;
		return;
	}

	if (m_bInReload)
		m_bInReload = false;

	// MUST call sound before removing a round from the clip of a CMachineGun
	BaseClass::WeaponSound( WPN_DOUBLE );

	pPlayer->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAGS_NONE );

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

	//Create the grenade
	QAngle angles;
	VectorAngles( vecThrow, angles );
	CGrenadeAR2 *pGrenade = (CGrenadeAR2*)Create( "grenade_ar2", vecSrc, angles, pPlayer );
	pGrenade->SetAbsVelocity( vecThrow );

	pGrenade->SetLocalAngularVelocity( RandomAngle( -400, 400 ) );
	pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
	pGrenade->SetThrower( GetOwner() );
	pGrenade->SetDamage( sk_plr_dmg_smg1_grenade.GetFloat() );

	SendWeaponAnim( ACT_VM_SECONDARYATTACK );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 1000, 0.2, GetOwner(), SOUNDENT_CHANNEL_WEAPON );

	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// Decrease ammo
	pPlayer->RemoveAmmo( 1, m_iSecondaryAmmoType );

	// Can shoot again immediately
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;

	// Can blow up after a short delay (so have time to release mouse button)
	m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;

	// Register a muzzleflash for the AI.
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	m_iSecondaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, false, GetClassname() );
}

//-----------------------------------------------------------------------------
// Purpose: De-scope OICW on reload
//-----------------------------------------------------------------------------
bool CWeaponOICW::Reload() {
	bool reloadBool = BaseClass::Reload();
	if (GetOwner()->IsPlayer()) {
		if (m_bIsScoped && reloadBool) {
			Unscope();
		}
	}
	return reloadBool;
}

//-----------------------------------------------------------------------------
// Purpose: Scope and de-scope the weapon
//-----------------------------------------------------------------------------
void CWeaponOICW::ToggleScope( void ) {
	if (GetOwner()->IsPlayer()) {
		if (GetActivity() == ACT_VM_RELOAD || GetOwner() != UTIL_GetLocalPlayer())
			return;

		if (m_flScopeTime >= gpGlobals->curtime) {
			return;
		}
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
		if (!pPlayer)
			return;

		if (m_bIsScoped)
			Unscope();
		else
			Scope();

		m_flScopeTime = gpGlobals->curtime + 0.4f;
	}
}

void CWeaponOICW::Scope( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if (!pPlayer)
		return;

	if (pPlayer->SetFOV( this, 30, 0.2f ))
	{
		UTIL_ScreenFade( pPlayer, { 0, 0, 0, 255 }, 0.4, 0, FFADE_IN );
		if (!m_bIsScoped)
		{
			pPlayer->ShowViewModel( false );
		}

		WeaponSound( SPECIAL1 );

		m_bIsScoped = true;
	}
}

void CWeaponOICW::Unscope( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if (!pPlayer)
		return;

	if (pPlayer->SetFOV( this, 0 ))
	{
		pPlayer->ShowViewModel( true );

		// Zoom out to the default zoom level
		WeaponSound( SPECIAL2 );
		m_bIsScoped = false;

	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOperator - 
//-----------------------------------------------------------------------------
void CWeaponOICW::FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, bool bUseWeaponAngles )
{
	Vector vecShootOrigin, vecShootDir;

	CAI_BaseNPC *npc = pOperator->MyNPCPointer();
	ASSERT( npc != NULL );

	if (bUseWeaponAngles)
	{
		QAngle	angShootDir;
		GetAttachment( LookupAttachment( "muzzle" ), vecShootOrigin, angShootDir );
		AngleVectors( angShootDir, &vecShootDir );
	}
	else
	{
		vecShootOrigin = pOperator->Weapon_ShootPosition();
		vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );
	}

	WeaponSoundRealtime( SINGLE_NPC );

	CSoundEnt::InsertSound( SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOperator, SOUNDENT_CHANNEL_WEAPON, pOperator->GetEnemy() );

	pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2 );

	// NOTENOTE: This is overriden on the client-side
	// pOperator->DoMuzzleFlash();

	m_iClip1 = m_iClip1 - 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponOICW::Operator_ForceNPCFire( CBaseCombatCharacter *pOperator, bool bSecondary )
{
	if (!bSecondary)
	{
		// Ensure we have enough rounds in the clip
		m_iClip1++;

		FireNPCPrimaryAttack( pOperator, true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEvent - 
//			*pOperator - 
//-----------------------------------------------------------------------------
void CWeaponOICW::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	switch (pEvent->event)
	{
	case EVENT_WEAPON_AR2:
	{
		FireNPCPrimaryAttack( pOperator, false );
	}
	break;

	default:
		CBaseCombatWeapon::Operator_HandleAnimEvent( pEvent, pOperator );
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponOICW::AddViewKick( void )
{
#define	EASY_DAMPEN			0.5f
#define	MAX_VERTICAL_KICK	8.0f	//Degrees
#define	SLIDE_LIMIT			5.0f	//Seconds

	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (!pPlayer)
		return;

	float flDuration = m_fFireDuration;

	if (g_pGameRules->GetAutoAimMode() == AUTOAIM_ON_CONSOLE)
	{
		// On the 360 (or in any configuration using the 360 aiming scheme), don't let the
		// OICW progressive into the late, highly inaccurate stages of its kick. Just
		// spoof the time to make it look (to the kicking code) like we haven't been
		// firing for very long.
		flDuration = MIN( flDuration, 0.75f );
	}

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, flDuration, SLIDE_LIMIT );
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CWeaponOICW::GetProficiencyValues()
{
	static WeaponProficiencyInfo_t proficiencyTable[] =
	{
		{ 7.0, 0.75 },
		{ 5.00, 0.75 },
		{ 3.0, 0.85 },
		{ 5.0 / 3.0, 0.75 },
		{ 1.00, 1.0 },
	};

	COMPILE_TIME_ASSERT( ARRAYSIZE( proficiencyTable ) == WEAPON_PROFICIENCY_PERFECT + 1 );

	return proficiencyTable;
}
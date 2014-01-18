#include "edf01.h"

#define TURQUOISE RGB(0,255,255)
#define YELLOW RGB(255,255,0)
#define BLUE RGB(0,0,255)
#define VIOLET RGB(255,0,255)

char* OBJECTIVE_TEXT_1 = "Your vehicle is equipped with\na special scanning device.\nScan the power source by\ngetting near it.";
char* OBJECTIVE_TEXT_2 = "Hostiles are moving to attack!!\nTake action!";
char* OBJECTIVE_TEXT_3 = "Have your subordinate tanks pick\nup the survivors, and bring them\nback to the dropships.  Only the\nsubordinate Sabres can get them.";
char* OBJECTIVE_TEXT_4 = "Tanks carrying survivors will be\nplaced in Group 10 automatically.\nThey can't pick you up while\ncarrying a survivor.  Don't lose\nany survivors!";
char* OBJECTIVE_TEXT_5 = "Scan the next highlighted\npower source.";
char* OBJECTIVE_TEXT_6 = "You've lost too many tanks\nto complete this mission.";
char* OBJECTIVE_TEXT_7 = "That's two survivors dead.  That\nlast one was no ordinary survivor,\nbut General Hardin himself!";
char* OBJECTIVE_TEXT_8 = "The Hadeans have overrun the\narea.  There's no hope now.";
char* OBJECTIVE_TEXT_9 = "Congratulations.  All the\nsurvivors are safe.  General\nHardin was among them, and\nwould like to thank you.";
char* OBJECTIVE_TEXT_10 = "A survivor was KILLED.  We\nCANNOT afford to lose another!!!";
char* OBJECTIVE_TEXT_11 = "All the remaining survivors are\nsafe.  We lost one, regretably.";

#define ODF_FRIEND_TANK "ivtank"
#define ODF_FRIEND_TANK_SURVIVOR "ivtank_e01"
#define ODF_FRIEND_TURRET "ivturr"
#define ODF_FRIEND_APC "ivapc"
#define ODF_ENEMY_TANK "evtank"
#define ODF_ENEMY_SCOUT "evscout"
#define ODF_ENEMY_MISL "evmisl"
#define ODF_SHULTZ_PILOT "ispilo"
#define ODF_SURVIVOR_PILOT "ispilo"

#define AUDIO_01 "edf01_01.wav"
#define AUDIO_02 "edf01_02.wav"
#define AUDIO_03 "edf01_03.wav"
#define AUDIO_04 "edf01_04.wav"
#define AUDIO_05 "edf01_05.wav"
#define AUDIO_05A "edf01_05A.wav"
#define AUDIO_05B "edf01_05B.wav"
#define AUDIO_06 "edf01_06.wav"
#define AUDIO_06A "edf01_06A.wav"
#define AUDIO_07 "edf01_07.wav"
#define AUDIO_08 "edf01_08.wav"
#define AUDIO_09 "edf01_09.wav"
#define AUDIO_10 "edf01_10.wav"
#define AUDIO_TANK_PICKUP "ivtank03.wav" // "I've got em"

#define TEXT_WIN1 "edf01W1.txt"
#define TEXT_WIN2 "edf01W2.txt"
#define TEXT_LOSS1 "edf01L1.txt"
#define TEXT_LOSS2 "edf01L2.txt"
#define TEXT_LOSS3 "edf01L3.txt"

#define INT_SCAN_DISTANCE 190 // Minimum distance for scanning
#define INT_SCAN_PER_SEC 40 // Percent per second * 10
#define INT_ENEMY_SATURATION_SLOWDOWN_COUNT 15 // At this number, enemy spawns slow down
#define INT_ENEMY_SATURATION_MAX_COUNT 24 // Loss if more than this many units spawned
#define INT_SURVIVOR_PICKUP_DISTANCE 24 // Minimum distance for picking up survivors
#define INT_SURVIVOR_DROPOFF_DISTANCE 85 // Minimum distance for dropping off survivors

void EDF01Mission::Init(void)
{
	if(i_array)
		memset(i_array, 0, i_count*sizeof(int));
	if(f_array)
		memset(f_array, 0, f_count*sizeof(float));
	if(h_array)
		memset(h_array, 0, h_count*sizeof(Handle));
	if(b_array)
		memset(b_array, 0, b_count*sizeof(bool));
	if(v_array)
		memset(v_array, 0, v_count*sizeof(Vector));

	EnableHighTPS(m_GameTPS);

	SetAutoGroupUnits(false);

	m_DidOneTimeInit = true;

	Preload();

	// TODO: revisit the camera code, get rid of these hard coded vectors
	m_ShultzEjectCameraBase = Vector(-10.0f,50.0f,-10.0f);
	//m_Gravity = GetVarItemInt("network.session.ivar31");
	// Set this for the server now. Clients get this set from Load().
	//SetGravity(static_cast<float>(m_Gravity) * 0.5f);

	m_SurvivorDropoffDropship = GetHandle("DropShip");

	// grab survivors
	for(int x=0; x<10; x++)
	{
		sprintf(TempMsgString, "Survivor%i",x);
		m_Survivors[x] = GetHandle(TempMsgString);
		SetMaxHealth(m_Survivors[x],800);
		SetCurHealth(m_Survivors[x],800);
	}
}

void EDF01Mission::AddObject(Handle h)
{
	char ODFName[64];
	GetObjInfo(h, Get_CFG, ODFName);

	//if (IsOdf2(h,"ibnav"))
	//{
	//	nav = h;
	//	// [N1] is this safe?  maybe the thing isn't too complex and it can be removed when added?
	//	RemoveObject(nav);
	//}
	//else if (IsOdf2(h,"fspilo"))
	//{
	//	pilot = h;
	//	Damage(pilot,101); // [N1] GSH indicates that the death check is <, not <=. Ejection invulnerability might be preventing them from getting an extra 0.1 damage
	//	//Damage(pilot,100); // [N1] changed
	//	//RemoveObject(pilot);
	//}
	
	if(m_ShultzTank)
	{
		if(IsOdf(h,ODF_SHULTZ_PILOT)
		&& !IsPlayer(h)
		&& IsAround(m_ShultzTank)
		&& !IsAlive2(m_ShultzTank))
		{
			m_ShultzPilot = h;
			m_ShultzTank = 0;
			m_ShutlzPilotActionWaitTillTime = m_ElapsedGameTime + (m_GameTPS * 2);
		}
	}

	if(m_SurvivorChecksActive)
	{
		if(IsOdf(h,ODF_ENEMY_TANK) || IsOdf(h,ODF_ENEMY_SCOUT))
		{
			if(GetTeamNum(h)==5)
			{
				m_AttackerCountForFailCheck++;
			}
		}
	}
}

void EDF01Mission::DeleteObject(Handle h)
{
	if(m_SurvivorChecksActive)
	{
		if(IsOdf(h,ODF_ENEMY_TANK) || IsOdf(h,ODF_ENEMY_SCOUT))
		{
			if(GetTeamNum(h)==5)
			{
				m_AttackerCountForFailCheck--;
			}
		}
	}
}

void EDF01Mission::Preload()
{
	PreloadODF(ODF_FRIEND_TANK);
	PreloadODF(ODF_FRIEND_TANK_SURVIVOR);
	PreloadODF(ODF_FRIEND_TURRET);
	PreloadODF(ODF_FRIEND_APC);
	PreloadODF(ODF_ENEMY_TANK);
	PreloadODF(ODF_ENEMY_SCOUT);
	PreloadODF(ODF_ENEMY_MISL);
	PreloadODF(ODF_SHULTZ_PILOT);
	PreloadODF(ODF_SURVIVOR_PILOT);

	PreloadAudioMessage(AUDIO_01);
	PreloadAudioMessage(AUDIO_02);
	PreloadAudioMessage(AUDIO_03);
	PreloadAudioMessage(AUDIO_04);
	PreloadAudioMessage(AUDIO_05);
	PreloadAudioMessage(AUDIO_05A);
	PreloadAudioMessage(AUDIO_05B);
	PreloadAudioMessage(AUDIO_06);
	PreloadAudioMessage(AUDIO_06A);
	PreloadAudioMessage(AUDIO_07);
	PreloadAudioMessage(AUDIO_08);
	PreloadAudioMessage(AUDIO_09);
	PreloadAudioMessage(AUDIO_10);
	PreloadAudioMessage(AUDIO_TANK_PICKUP);
}

void EDF01Mission::MakeNavs()
{
	if(!IsAround(m_SurvivorDropoffNav) || !m_SurvivorDropoffNav)
	{
		m_SurvivorDropoffNav = BuildObject("ibnav",1,"SafeNav");
		SetObjectiveName(m_SurvivorDropoffNav,"Survivor Dropoff");
		SetObjectiveOn(m_SurvivorDropoffNav);
	}
	if(!IsAround(m_SurvivorNav1) || !m_SurvivorNav1)
	{
		m_SurvivorNav1 = BuildObject("ibnav",1,"SurvivorNav1");
		SetObjectiveName(m_SurvivorNav1,"Survivors");
		SetObjectiveOn(m_SurvivorNav1);
	}
	if(!IsAround(m_SurvivorNav2) || !m_SurvivorNav2)
	{
		m_SurvivorNav2 = BuildObject("ibnav",1,"SurvivorNav2");
		SetObjectiveName(m_SurvivorNav2,"Survivors");
		SetObjectiveOn(m_SurvivorNav2);
	}
}

bool EDF01Mission::Load(bool missionSave)
{
	SetAutoGroupUnits(false);
	EnableHighTPS(m_GameTPS);

	// Always do this to hook up clients with the taunt engine as well.
	//InitTaunts(&m_ElapsedGameTime, &m_LastTauntPrintedAt, &m_GameTPS, "Bots");

	// We're a 1.3 DLL.
	//WantBotKillMessages();

	// Do this for everyone as well.
	//ClearObjectives();
	//AddObjective("mpobjective_dm.otf", WHITE, -1.0f); // negative time means don't change display to show it

	if (missionSave)
	{
		int i;

		// init bools
		if((b_array) && (b_count))
			for (i = 0; i < b_count; i++)
				b_array[i] = false;

		// init floats
		if((f_array) && (f_count))
			for (i = 0; i < f_count; i++)
				f_array[i] = 0.0f;

		// init handles
		if((h_array) && (h_count))
			for (i = 0; i < h_count; i++)
				h_array[i] = 0;

		// init ints
		if((i_array) && (i_count))
			for (i = 0; i < i_count; i++)
				i_array[i] = 0;

		// init Vectors
		if((v_array) && (v_count))
			for (i = 0; i < v_count; i++)
				v_array[i] = Vector();

		return true;
	}

	bool ret = true;

	// bools
	if (b_array != NULL)
		ret = ret && Read(b_array, b_count);

	// floats
	if (f_array != NULL)
		ret = ret && Read(f_array, f_count);

	// Handles
	if (h_array != NULL)
		ret = ret && Read(h_array, h_count);

	// ints
	if (i_array != NULL)
		ret = ret && Read(i_array, i_count);

	// Vectors
	if (v_array != NULL)
		ret = ret && Read(v_array, v_count);

	// Set this right after reading -- we might be on a client.  Safe
	// to call this multiple times on the server.
	//SetGravity(static_cast<float>(m_Gravity) * 0.5f);

	//PUPMgr::Load(missionSave);

	Preload();
	return ret;
}


bool EDF01Mission::PostLoad(bool missionSave)
{
	if (missionSave)
		return true;

	bool ret = true;

	ConvertHandles(h_array, h_count);

	//ret = ret && PUPMgr::PostLoad(missionSave);
	return ret;
}


bool EDF01Mission::Save(bool missionSave)
{
	if (missionSave)
		return true;

	bool ret = true;

	// bools
	if (b_array != NULL)
		ret = ret && Write(b_array, b_count);

	// floats
	if (f_array != NULL)
		ret = ret && Write(f_array, f_count);

	// Handles
	if (h_array != NULL)
		ret = ret && Write(h_array, h_count);

	// ints
	if (i_array != NULL)
		ret = ret && Write(i_array, i_count);

	// Vectors
	if (v_array != NULL)
		ret = ret && Write(v_array, v_count);

	//ret = ret && PUPMgr::Save(missionSave);

	return ret;
}

void EDF01Mission::Execute(void)
{
	if (!m_DidOneTimeInit)
		Init();

	/*if (!m_PlayerCanMove)
	{
		VehicleControls controls;
		controls.braccel = 0;
		controls.strafe = 0;
		controls.jump = 0;
		controls.deploy = 0;
		controls.eject = 0;
		controls.abandon = 0;
		controls.fire = 0;
		SetControls(Object_Player, controls,
			CTRL_BRACCEL +
			CTRL_STRAFE +
			CTRL_JUMP +
			CTRL_DEPLOY +
			CTRL_EJECT +
			CTRL_ABANDON +
			CTRL_FIRE
			);
	}*/

	Handle localPlayer = GetPlayerHandle();
	Handle tmpHandle = 0;

	if(m_ForceNavsToStay)
	{
		MakeNavs();
	}

	if(m_ShultzPilot) // we are tracking his pilot, he ejected
	{
		if(m_ShutlzCamActive)
		{
			CameraObject(m_SurvivorDropoffDropship,m_ShultzEjectCameraBase.x,m_ShultzEjectCameraBase.y,m_ShultzEjectCameraBase.z,m_ShultzPilot);
		}
		if(m_ElapsedGameTime >= m_ShutlzPilotActionWaitTillTime)
		{
			if(m_ShutlzCamActive)
			{
				CameraFinish();
				m_ShutlzPilotActionWaitTillTime = m_ElapsedGameTime + (m_GameTPS * 2);
				m_ShutlzCamActive = false;
			}else if(m_ShutlzGoingToDropship)
			{
				RemoveObject(m_ShultzPilot);
				m_ShultzPilot = 0;
			}else{
				Goto(m_ShultzPilot,m_SurvivorDropoffDropship,1);
				
				CameraReady();
				CameraObject(m_SurvivorDropoffDropship,m_ShultzEjectCameraBase.x,m_ShultzEjectCameraBase.y,m_ShultzEjectCameraBase.z,m_ShultzPilot);
				m_ShutlzPilotActionWaitTillTime = m_ElapsedGameTime + (m_GameTPS * 18);
				m_ShutlzGoingToDropship = true;
				m_ShutlzCamActive = true;
			}
		}
	}

	if (m_ElapsedGameTime >= m_ScanningMissionPhaseWaitTillTime)
	{
		switch(m_ScanningMissionPhase)
		{
		case 0: // any special setup, could probably replace with 1 time init part
			m_ScanningMissionPhase++;
			break;
		case 1:
			SetPerceivedTeam(localPlayer,5);

			m_FriendTurret3 = BuildObject(ODF_FRIEND_TURRET,1,"FriendTurret3");
			m_FriendTurret2 = BuildObject(ODF_FRIEND_TURRET,1,"FriendTurret2");
			tmpHandle = BuildObject(ODF_FRIEND_TURRET,1,"FriendTurret1");

			SetGroup(m_FriendTurret3,1);
			SetGroup(m_FriendTurret2,1);

			Stop(m_FriendTurret3,1);
			Stop(m_FriendTurret2,1);
			Stop(tmpHandle,1);
			
			m_APC1 = BuildObject(ODF_FRIEND_APC,1,"APC1");
			m_APC2 = BuildObject(ODF_FRIEND_APC,1,"APC2");
			Stop(m_APC1,1);
			Stop(m_APC2,1);

			m_BuddyTanks[0] = BuildObject(ODF_FRIEND_TANK,1,"Buddy1");
			m_BuddyTanks[1] = BuildObject(ODF_FRIEND_TANK,1,"Buddy2");
			m_BuddyTanks[2] = BuildObject(ODF_FRIEND_TANK,1,"Buddy3");
			m_BuddyTanks[3] = BuildObject(ODF_FRIEND_TANK,1,"Buddy4");
			m_BuddyTanks[4] = BuildObject(ODF_FRIEND_TANK,1,"Buddy5");
			m_BuddyTanks[5] = BuildObject(ODF_FRIEND_TANK,1,"Buddy6");

			Stop(m_BuddyTanks[0],1);
			Stop(m_BuddyTanks[1],1);
			Stop(m_BuddyTanks[2],1);
			Stop(m_BuddyTanks[3],1);
			Stop(m_BuddyTanks[4],1);
			Stop(m_BuddyTanks[5],1);

			SetPerceivedTeam(m_BuddyTanks[0],5);
			SetPerceivedTeam(m_BuddyTanks[1],5);
			SetPerceivedTeam(m_BuddyTanks[2],5);
			SetPerceivedTeam(m_BuddyTanks[3],5);
			SetPerceivedTeam(m_BuddyTanks[4],5);
			SetPerceivedTeam(m_BuddyTanks[5],5);

			m_ShultzTank = BuildObject(ODF_FRIEND_TANK,1,"Dummy");
			Stop(m_ShultzTank,1);
			SetObjectiveName(m_ShultzTank,"Schulz");
			SetObjectiveOn(m_ShultzTank);

			m_ScanningMissionPhaseWaitTillTime = m_ElapsedGameTime + (4 * m_GameTPS);
			m_ScanningMissionPhase++;
			break;
		case 2:
			AudioMessage(AUDIO_01);
			Goto(m_APC1,"APCDest",1);
			Goto(m_APC2,"APCDest",1);
			m_ScanningMissionPhaseWaitTillTime = m_ElapsedGameTime + (17 * m_GameTPS);
			m_ScanningMissionPhase++;
			break;
		case 3:
			EjectPilot(m_ShultzTank);
			m_ScanningMissionPhaseWaitTillTime = m_ElapsedGameTime + (3 * m_GameTPS);
			m_ScanningMissionPhase++;
			break;
		case 4:
			AudioMessage(AUDIO_02);
			AddObjective(OBJECTIVE_TEXT_1,WHITE);
			AudioMessage(AUDIO_03);
			m_ScanningMissionPhase++;
			break;
		case 5: // move to target for scanning
			{
				sprintf_s(TempMsgString, "Nav%iA", m_PowerSourceNumber);
				Goto(m_BuddyTanks[0],TempMsgString,1);
				Goto(m_BuddyTanks[2],TempMsgString,1);
				Goto(m_BuddyTanks[4],TempMsgString,1);

				sprintf_s(TempMsgString, "Nav%iB", m_PowerSourceNumber);
				Goto(m_BuddyTanks[1],TempMsgString,1);
				Goto(m_BuddyTanks[3],TempMsgString,1);
				Goto(m_BuddyTanks[5],TempMsgString,1);

				sprintf_s(TempMsgString, "PowerSource%i", m_PowerSourceNumber);
				m_TargetPowerSourceForScanning = GetHandle(TempMsgString);
				SetObjectiveOn(m_TargetPowerSourceForScanning);

				m_ScanningMissionPhase++;
			}
		case 6: // do scanning
			{
				sprintf_s(TempMsgString, "PowerSource%i", m_PowerSourceNumber);
				m_TargetPowerSourceForScanning = GetHandle(TempMsgString);
				sprintf_s(TempMsgString,"Power source: %i%% scanned",(m_ScannedPercent/10));
				SetObjectiveName(m_TargetPowerSourceForScanning,TempMsgString);

				Dist dist = GetDistance(m_TargetPowerSourceForScanning,localPlayer);
				if (dist <= INT_SCAN_DISTANCE)
				{
					m_ScannedPercent += (INT_SCAN_PER_SEC / m_GameTPS);//+= 4;
					sprintf_s(TempMsgString,"Power source: %i%% scanned",(m_ScannedPercent/10));
					SetObjectiveName(m_TargetPowerSourceForScanning,TempMsgString);

					if(m_ScannedPercent >= 1000)
					{
						SetObjectiveOff(m_TargetPowerSourceForScanning);
						m_PowerSourceNumber++;
						m_ScannedPercent = 0;
						if(m_PowerSourceNumber <= 1)
						{
							ClearObjectives();
							AddObjective(OBJECTIVE_TEXT_5,WHITE);
							m_ScanningMissionPhase = 5;
						}else{
							m_ScanningMissionPhaseWaitTillTime = m_ElapsedGameTime + (61 * m_GameTPS);
							m_ScanningMissionPhase++;
						}
					}
				}
			}
			break;
		case 7:
			tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn1");
			Patrol(tmpHandle,"EnemyPatrol1",0);
			tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn1");
			Patrol(tmpHandle,"EnemyPatrol1",0);
			m_ScanningMissionPhaseWaitTillTime = m_ElapsedGameTime + (67 * m_GameTPS);
			m_ScanningMissionPhase++;
			break;
		case 8:
			tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn1");
			Patrol(tmpHandle,"EnemyPatrol1",0);
			m_ScanningMissionPhaseWaitTillTime = m_ElapsedGameTime + (61 * m_GameTPS);
			m_ScanningMissionPhase++;
			break;
		case 9:
			tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn1");
			Patrol(tmpHandle,"EnemyPatrol1",0);
			m_ScanningMissionPhase++;
			break;
		case 10:
			m_SpawnAttackerLoop = true;
			m_ScanningMissionPhase++;
			break;
		}
	}

	if(m_SpawnAttackerLoop)
	{
		if(m_SurvivorSafeCount == 0)
		{
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime)
			{
				switch(m_SpawnAttackerLoopStateMachine)
				{
				case 0:
					m_SpawnAttackerLoopWaitTillTime = m_ElapsedGameTime + (75 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine++;
					break;
				case 2:
					tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime = m_ElapsedGameTime + (95 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine++;
					break;
				case 3:
					tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime = m_ElapsedGameTime + (75 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine++;
					break;
				case 4:
					tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopStateMachine = 0;
					break;
				}
			}
		}else{
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
			{
				switch(m_SpawnAttackerLoopStateMachine2)
				{
				case 0:
					m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine2++;
					break;
				case 1:
					tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (95 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine2++;
					break;
				case 2:
					tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine2++;
					break;
				case 3:
					tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine2++;
					break;
				case 4:
					tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					tmpHandle = BuildObject(ODF_ENEMY_SCOUT,5,"AttackerSpawn2");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (95 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine2++;
					break;
				case 5:
					tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"TurretSpawn");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
					m_SpawnAttackerLoopStateMachine2++;
					break;
				case 6:
					tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"TurretSpawn");
					Patrol(tmpHandle,"EnemyPatrol1",0);
					if(m_AttackerCountForFailCheck < INT_ENEMY_SATURATION_SLOWDOWN_COUNT)
					{
						m_SpawnAttackerLoopStateMachine2 = 0;
					}else{
						m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (140 * m_GameTPS);
						m_SpawnAttackerLoopStateMachine2++;
					}
					break;
				case 7:
					m_SpawnAttackerLoopStateMachine2 = 0;
				}
			}
		}
	}

	if (m_ElapsedGameTime >= m_SurvivorMissionPhaseWaitTillTime)
	{
		switch(m_SurvivorMissionPhase)
		{
		case 0:
			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (40 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 1:
			m_HadeanObserver1 = BuildObject(ODF_ENEMY_TANK,5,"Observer1");
			LookAt(m_HadeanObserver1,localPlayer,0);
			m_HadeanObserver2 = BuildObject(ODF_ENEMY_TANK,5,"Observer2");
			LookAt(m_HadeanObserver2,localPlayer,0);
			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (10 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 2:
			RemoveObject(m_APC1);
			RemoveObject(m_APC2);
			m_SurvivorMissionPhase++;
			break;
		case 3:
			if(m_ElapsedGameTime > (200 * m_GameTPS)) // gametime is > 200 seconds
			{
				m_SurvivorMissionPhase++;
			}else if(m_PowerSourceNumber >= 1)
			{
				if(GetTeamNum(GetWhoShotMe(m_HadeanObserver1)) == 1)
				{
					m_SurvivorMissionPhase++;
				}else if(GetTeamNum(GetWhoShotMe(m_HadeanObserver2)) == 1)
				{
					m_SurvivorMissionPhase++;
				}
				else if(m_ScannedPercent >= 450)
				{
					m_SurvivorMissionPhase++;
				}
			}
			break;
		case 4:
			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (4 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 5:
			if (m_ElapsedGameTime >= m_SurvivorMissionPhaseWaitTillTime)
				m_SurvivorMissionPhase++;
			break;
		case 6:
			AudioMessage(AUDIO_04);
			SetPerceivedTeam(localPlayer,1);
			SetPerceivedTeam(m_BuddyTanks[0],1);
			SetPerceivedTeam(m_BuddyTanks[1],1);
			Attack(m_HadeanObserver1,m_BuddyTanks[0],0);
			Attack(m_HadeanObserver2,m_BuddyTanks[1],0);
			SetPerceivedTeam(m_BuddyTanks[0],1);
			SetPerceivedTeam(m_BuddyTanks[1],1);
			SetPerceivedTeam(m_BuddyTanks[2],1);
			SetPerceivedTeam(m_BuddyTanks[3],1);
			SetPerceivedTeam(m_BuddyTanks[4],1);
			SetPerceivedTeam(m_BuddyTanks[5],1);
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_2,WHITE);
			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (4 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 7:
			SetGroup(m_BuddyTanks[0],0);
			SetGroup(m_BuddyTanks[1],0);
			SetGroup(m_BuddyTanks[2],0);
			SetGroup(m_BuddyTanks[3],0);
			SetGroup(m_BuddyTanks[4],0);
			SetGroup(m_BuddyTanks[5],0);

			Stop(m_BuddyTanks[0],0);
			Stop(m_BuddyTanks[1],0);
			Stop(m_BuddyTanks[2],0);
			Stop(m_BuddyTanks[3],0);
			Stop(m_BuddyTanks[4],0);
			Stop(m_BuddyTanks[5],0);

			SetObjectiveOff(m_TargetPowerSourceForScanning);
			m_SurvivorMissionPhase++;
		case 8:
			if(!IsAliveAndPilot2(m_HadeanObserver1)
			&& GetTeamNum(m_HadeanObserver1) != 5
			&& !IsAliveAndPilot2(m_HadeanObserver2)
			&& GetTeamNum(m_HadeanObserver2) != 5)
				m_SurvivorMissionPhase++;
			break;
		case 9:
			m_ScanningMissionPhase = 7;
			m_SurvivorChecksActive = true;
			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (7 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 10:
			if (m_ElapsedGameTime >= m_SurvivorMissionPhaseWaitTillTime)
				m_SurvivorMissionPhase++;
			break;
		case 11:
			AudioMessage(AUDIO_05);
			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (14 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 12:
			AudioMessage(AUDIO_05A);
			m_ForceNavsToStay = true;
			MakeNavs();
			BuildObject(ODF_ENEMY_SCOUT,5,"Enemy2");
			BuildObject(ODF_ENEMY_SCOUT,5,"Enemy3");

			//////// Was part of a mysterious repeating routine in the BZS ////////
			ClearObjectives();			AddObjective(OBJECTIVE_TEXT_3,WHITE);			AddObjective(OBJECTIVE_TEXT_4,WHITE);
			///////////////////////////////////////////////////////////////////////

			m_SurvivorMissionPhaseWaitTillTime = m_ElapsedGameTime + (140 * m_GameTPS);
			m_SurvivorMissionPhase++;
			break;
		case 13: // LOC_209:
			{
				Dist dist = GetDistance(m_SurvivorDropoffDropship,localPlayer);
				if(dist >= 200)
					m_SurvivorMissionPhase++;
			}
			break;
		case 14:
			tmpHandle = BuildObject("evturr",5,"TurretSpawn");
			Goto(tmpHandle,"Turret1",1);
			tmpHandle = BuildObject("evturr",5,"TurretSpawn");
			Goto(tmpHandle,"Turret2",1);
			tmpHandle = BuildObject("evturr",5,"TurretSpawn");
			Goto(tmpHandle,"Turret3",1);
			Stop(m_FriendTurret3,0);
			Stop(m_FriendTurret2,0);
			tmpHandle = BuildObject("ivserv",1,"Buddy4");
			SetGroup(tmpHandle,2);
			tmpHandle = BuildObject("ivserv",1,"Buddy5");
			SetGroup(tmpHandle,3);
			tmpHandle = BuildObject("ivserv",1,"Buddy6");
			SetGroup(tmpHandle,4);
			AudioMessage(AUDIO_05B);
			m_SurvivorMissionPhase++;
			break;
		}
	}

	if(m_SurvivorChecksActive)
	{
		if (m_ElapsedGameTime >= m_SurvivorChecksWaitTillTime)
		{
			int SurvivorsMissing = 0;
			int SurvivorsLost = 0;
			int SurvivorsRescued = 0;
			int SurvivorsTransit = 0;
			for(int x = 0; x < 10; x++)
			{
				Handle survivor = m_Survivors[x];
				if(!survivor)
				{
					SurvivorsLost++;
				}else if(!IsAlive2(survivor))
				{
					m_Survivors[x] = 0; // this should have happened anyway
					SurvivorsLost++;
				}else if(IsOdf(survivor, ODF_SURVIVOR_PILOT))
				{
					Dist dist = INT_SURVIVOR_PICKUP_DISTANCE + 1;
					
					int BuddyIndex = -1;
					for(int y=0;y<6;y++)
					{
						Dist findingNearest = GetDistance(m_BuddyTanks[y],survivor);
						if(findingNearest < dist)
						{
							dist = findingNearest;
							BuddyIndex = y;
						}
					}

					if(m_BuddyTanks[BuddyIndex] && !IsPlayer(m_BuddyTanks[BuddyIndex]) && dist <= INT_SURVIVOR_PICKUP_DISTANCE && IsOdf(m_BuddyTanks[BuddyIndex], ODF_FRIEND_TANK) && BuddyIndex >= 0)
					{
						RemoveObject(m_Survivors[x]);
						m_Survivors[x] = ReplaceObject(m_BuddyTanks[BuddyIndex], ODF_FRIEND_TANK_SURVIVOR, true);
						m_BuddyTanks[BuddyIndex] = m_Survivors[x];
						AudioMessage(AUDIO_TANK_PICKUP);
						sprintf(TempMsgString,"Has survivor %i",++m_SurvivorNumberForBuddyLabeling); // prefixed addition so it occurs before the var is used in function
						SetObjectiveName(m_Survivors[x],TempMsgString);
						SetObjectiveOn(m_Survivors[x]);
						SetGroup(m_Survivors[x],9);
				
						//m_SurvivorChecksWaitTillTime = m_ElapsedGameTime + (1 * m_GameTPS); //Wait,1
						//Follow(m_Survivors[x],GetPlayerHandle(),0);

						SurvivorsTransit++;
					}else{
						SurvivorsMissing++;
					}
				}else if(IsOdf(survivor, ODF_FRIEND_TANK_SURVIVOR))
				{
					Dist dist = GetDistance(survivor,"SafeNav");

					int BuddyIndex = -1;
					for(int y=0;y<6;y++)
					{
						if(m_BuddyTanks[y] == survivor)
						{
							BuddyIndex = y;
							break;
						}
					}

					if(dist <= INT_SURVIVOR_DROPOFF_DISTANCE && BuddyIndex >= 0)
					{
						m_BuddyTanks[BuddyIndex] = ReplaceObject(survivor, ODF_FRIEND_TANK, true);
						SetGroup(m_BuddyTanks[BuddyIndex],0);

						m_Survivors[x] = m_SurvivorDropoffDropship;

						sprintf(TempMsgString,"Survivor Dropoff: %i safe",SurvivorsRescued);
						SetObjectiveName(m_SurvivorDropoffNav,TempMsgString);

						SurvivorsRescued++;
					}else{
						SurvivorsTransit++;
					}
				}else if(survivor == m_SurvivorDropoffDropship)
				{
					SurvivorsRescued++;
				}
			}

			m_SurvivorSafeCount = SurvivorsRescued;

			if(m_SurvivorNav1ShowCount)
			{
				int SurvivorsAtNav1 = 0;
				for(int x = 0; x < 5; x++)
				{
					Handle survivor = m_Survivors[x];
					if(survivor && IsAlive2(survivor) && IsOdf(survivor, ODF_SURVIVOR_PILOT))
					{
						Dist dist = GetDistance(m_SurvivorNav1,survivor);
						if(dist < 50)
						{
							SurvivorsAtNav1++;
						}
					}
				}

				if(SurvivorsAtNav1 > 0)
				{
					if(SurvivorsMissing > 1)
					{
						sprintf(TempMsgString,"%i Survivors Waiting",SurvivorsAtNav1);
						SetObjectiveName(m_SurvivorNav1,TempMsgString);
					}else if(SurvivorsMissing == 1)
					{
						SetObjectiveName(m_SurvivorNav1,"Last Waiting Survivor");
					}
				}else{
					SetObjectiveName(m_SurvivorNav1,"All Survivors Rescued");
				}
			}else{
				Dist dist = GetDistance(m_SurvivorNav1,localPlayer);
				if(dist < 100)
				{
					m_SurvivorNav1ShowCount = true;
				}
			}
			
			if(m_SurvivorNav2ShowCount)
			{
				int SurvivorsAtNav2 = 0;
				for(int x = 5; x < 10; x++)
				{
					Handle survivor = m_Survivors[x];
					if(survivor && IsAlive2(survivor) && IsOdf(survivor, ODF_SURVIVOR_PILOT))
					{
						Dist dist = GetDistance(m_SurvivorNav2,survivor);
						if(dist < 50)
						{
							SurvivorsAtNav2++;
						}
					}
				}

				if(SurvivorsAtNav2 > 0)
				{
					if(SurvivorsMissing > 1)
					{
						sprintf(TempMsgString,"%i Survivors Waiting",SurvivorsAtNav2);
						SetObjectiveName(m_SurvivorNav2,TempMsgString);
					}else if(SurvivorsMissing == 1)
					{
						SetObjectiveName(m_SurvivorNav2,"Last Waiting Survivor");
					}
				}else{
					SetObjectiveName(m_SurvivorNav2,"All Survivors Rescued");
				}
			}else{
				Dist dist = GetDistance(m_SurvivorNav2,localPlayer);
				if(dist < 100)
				{
					m_SurvivorNav2ShowCount = true;
				}
			}

			//if(SurvivorsRescued == 9 && SurvivorsLost == 1)
			//{
			//	ClearObjectives();
			//	AddObjective(OBJECTIVE_TEXT_11,GREEN);
			//	AudioMessage(AUDIO_06);
			//	SucceedMission(GetTime() + 15,TEXT_WIN2);
			//	m_SurvivorChecksActive = false;
			//}

			if(SurvivorsRescued == 10)
			{
				AudioMessage(AUDIO_06);
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_9,GREEN);
				SucceedMission(GetTime() + 15,TEXT_WIN1);
				m_SurvivorChecksActive = false;
			}else if(SurvivorsRescued == 9 && SurvivorsLost == 0)
			{
				tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn2");
				Goto(tmpHandle,"SafeNav",0);
				tmpHandle = BuildObject(ODF_ENEMY_MISL,5,"AttackerSpawn2");
				Goto(tmpHandle,"SafeNav",0);
				tmpHandle = BuildObject(ODF_ENEMY_MISL,5,"AttackerSpawn2");
				Goto(tmpHandle,"SafeNav",0);
			}else if(SurvivorsRescued >= 3 && !m_FirstResponsiveAttackWave)
			{
				tmpHandle = BuildObject(ODF_ENEMY_TANK,5,"AttackerSpawn2");
				Goto(tmpHandle,"SafeNav",0);
				tmpHandle = BuildObject(ODF_ENEMY_MISL,5,"AttackerSpawn2");
				Goto(tmpHandle,"SafeNav",0);
				tmpHandle = BuildObject(ODF_ENEMY_MISL,5,"AttackerSpawn2");
				Goto(tmpHandle,"SafeNav",0);
				m_FirstResponsiveAttackWave = true;
			}
			
			if(SurvivorsLost == 1)
			{
				if(!m_NotedLostSurvivor)
				{
					ClearObjectives();
					AddObjective(OBJECTIVE_TEXT_10,RED);
					AudioMessage(AUDIO_06A);
					
					m_NotedLostSurvivor = true;
					m_SurvivorChecksWaitTillTime = m_ElapsedGameTime + (10 * m_GameTPS); //Wait,10
				}else if(SurvivorsRescued == 9)// && !m_NoteAllRescued)
				{
					ClearObjectives();
					AddObjective(OBJECTIVE_TEXT_11,GREEN);
					AudioMessage(AUDIO_06);
					SucceedMission(GetTime() + 15,TEXT_WIN2);
					m_SurvivorChecksActive = false;
					//m_NoteAllRescued = true; // not needed as it would never hit here again due to survivor checks
				}
			}else if(SurvivorsLost >= 2)// && !m_NoteLossOfTwoSurvivors)
			{
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_7,RED);
				AudioMessage(AUDIO_07);
				FailMission(GetTime() + 15,TEXT_LOSS2);
				m_SurvivorChecksActive = false;
				//m_NoteLossOfTwoSurvivors = true; // not needed as it would never hit here again due to survivor checks
			}

			int CountTanks = 0;
			for(int y=0;y<6;y++)
			{
				if(IsAlive2(m_BuddyTanks[y]))
				{
					CountTanks++;
				}
			}
			if(CountTanks == 0)
			{
				if(!m_StepOneOfTankCountFailDone)
				{
					ClearObjectives();
					AddObjective(OBJECTIVE_TEXT_6,RED);
					AudioMessage(AUDIO_09);

					m_StepOneOfTankCountFailDone = true;
					m_SurvivorChecksWaitTillTime = m_ElapsedGameTime + (9 * m_GameTPS); //Wait,9
				}else{
					AudioMessage(AUDIO_10);
					FailMission(GetTime() + 14,TEXT_LOSS1);
					m_SurvivorChecksActive = false;
				}
			}

			// a bit of overlapping delays here but really we want the win check to be more agressive because we aren't asshats
			if (m_ElapsedGameTime >= m_AttackCountFailCheckWaitTillTime)
			{
				if(m_AttackerCountForFailCheck > INT_ENEMY_SATURATION_MAX_COUNT)
				{
					if(SurvivorsMissing >= 1)
					{
						if(!m_StepOneOfEnemySpawnCountFailDone)
						{
							ClearObjectives();
							AddObjective(OBJECTIVE_TEXT_8,RED);
							AudioMessage(AUDIO_08);
							m_AttackCountFailCheckWaitTillTime = m_ElapsedGameTime + (5 * m_GameTPS);//Wait,5
							m_StepOneOfEnemySpawnCountFailDone = true;
						}else{
							AudioMessage(AUDIO_10);
							FailMission(GetTime() + 12,TEXT_LOSS3);
							m_SurvivorChecksActive = false;
						}
					}
				}
			}
		}
	}

	m_ElapsedGameTime++;
}

Vector EDF01Mission::GetVectorFromPath(Name path, int point)
{
	Vector retVal;
	size_t bufSize = 0;
	float* pData = NULL;
	GetPathPoints(path, bufSize, pData);
	if(point >= UnsignedToSigned(bufSize))
		return Vector();
	pData = new float[2 * bufSize];
	if(GetPathPoints(path, bufSize, pData))
		retVal = Vector(pData[2*point+0], TerrainFindFloor(pData[2*point+0], pData[2*point+1]), pData[2*point+1]);
	delete[] pData;
	return retVal;
}

int EDF01Mission::UnsignedToSigned(unsigned int x)
{
    if (x <= INT_MAX)
        return static_cast<int>(x);

    //if (x >= INT_MIN)
	#pragma warning( push )
	#pragma warning( disable : 4308)
	if (x > INT_MIN - 1u) // MS compiler likes this more
	#pragma warning( pop ) 
        return static_cast<int>(x - INT_MIN) + INT_MIN;

    throw x; // Or whatever else you like
}

Handle EDF01Mission::ReplaceObject(Handle h, char* odf, bool keepHealth)
{
	int Team = GetTeamNum(h);
	Matrix M;
	GetPosition(h, M);
	long Health = GetCurHealth(h);
	bool WasPlayer = IsPlayer(h);

	if (IsAround(h))
	{
		RemoveObject(h);
	}

	strcpy(TempMsgString, odf);

	Handle retVal = BuildObject(TempMsgString, Team, M);

	if (WasPlayer)
	{
		SetAsUser(retVal, 1);
	}

	if (!keepHealth)
	{
		SetCurHealth(retVal, Health);
	}

	return retVal;
}



DLLBase * BuildMission(void)
{
	return new EDF01Mission();
}
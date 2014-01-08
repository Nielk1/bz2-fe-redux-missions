#include "edf01.h"

#define TURQUOISE RGB(0,255,255)
#define YELLOW RGB(255,255,0)
#define BLUE RGB(0,0,255)
#define VIOLET RGB(255,0,255)

char* _Text1 = "Your vehicle is equipped with\na special scanning device.\nScan the power source by\ngetting near it.";
char* _Text2 = "Hostiles are moving to attack!!\nTake action!";
char* _Text3 = "Have your subordinate tanks pick\nup the survivors, and bring them\nback to the dropships.  Only the\nsubordinate Sabres can get them.";
char* _Text4 = "Tanks carrying survivors will be\nplaced in Group 10 automatically.\nThey can't pick you up while\ncarrying a survivor.  Don't lose\nany survivors!";
char* _Text5 = "Scan the next highlighted\npower source.";
char* _Text6 = "You've lost too many tanks\nto complete this mission.";
char* _Text7 = "That's two survivors dead.  That\nlast one was no ordinary survivor,\nbut General Hardin himself!";
char* _Text8 = "The Hadeans have overrun the\narea.  There's no hope now.";
char* _Text9 = "Congratulations.  All the\nsurvivors are safe.  General\nHardin was among them, and\nwould like to thank you.";
char* _Text10 = "A survivor was KILLED.  We\nCANNOT afford to lose another!!!";
char* _Text11 = "All the remaining survivors are\nsafe.  We lost one, regretably.";

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

	// TODO: revisit the camera code, get rid of these hard coded vectors
	Position1 = Vector(-10.0f,50.0f,-10.0f);
	//m_Gravity = GetVarItemInt("network.session.ivar31");
	// Set this for the server now. Clients get this set from Load().
	//SetGravity(static_cast<float>(m_Gravity) * 0.5f);

	Variable10 = 8;

	Object_DropShip = GetHandle("DropShip");
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

	switch(m_mainStateMachine)
	{
	case 0: // any special setup, could probably replace with 1 time init part
		m_mainStateMachine++;
		break;
	case 1:
		Object_Player = GetPlayerHandle();
		SetPerceivedTeam(Object_Player,5);
		Object_FriendTurret3 = BuildObject("ivturr",1,"FriendTurret3");
		SetGroup(Object_FriendTurret3,1);
		Stop(Object_FriendTurret3,1);
		Object_FriendTurret2 = BuildObject("ivturr",1,"FriendTurret2");
		SetGroup(Object_FriendTurret2,1);
		Stop(Object_FriendTurret2,1);
		Object_TempCreation = BuildObject("ivturr",1,"FriendTurret1");
		Stop(Object_TempCreation,1);
		Object_APC1 = BuildObject("ivapc",1,"APC1");
		Stop(Object_APC1,1);
		Object_APC2 = BuildObject("ivapc",1,"APC2");
		Stop(Object_APC2,1);
		Object_Buddy1 = BuildObject("ivtank",1,"Buddy1");
		Stop(Object_Buddy1,1);
		SetPerceivedTeam(Object_Buddy1,5);
		Object_Buddy2 = BuildObject("ivtank",1,"Buddy2");
		Stop(Object_Buddy2,1);
		SetPerceivedTeam(Object_Buddy2,5);
		Object_Buddy3 = BuildObject("ivtank",1,"Buddy3");
		Stop(Object_Buddy3,1);
		SetPerceivedTeam(Object_Buddy3,5);
		Object_Buddy4 = BuildObject("ivtank",1,"Buddy4");
		Stop(Object_Buddy4,1);
		SetPerceivedTeam(Object_Buddy4,5);
		Object_Buddy5 = BuildObject("ivtank",1,"Buddy5");
		Stop(Object_Buddy5,1);
		SetPerceivedTeam(Object_Buddy5,5);
		Object_Buddy6 = BuildObject("ivtank",1,"Buddy6");
		Stop(Object_Buddy6,1);
		SetPerceivedTeam(Object_Buddy6,5);
		Object_TempCreation = BuildObject("ivtank",1,"Dummy");
		Stop(Object_TempCreation,1);
		SetObjectiveName(Object_TempCreation,"Schulz");
		SetObjectiveOn(Object_TempCreation);
		//RunSpeed,_Routine1,1,true,true
		m_mainWaitTillTime = m_ElapsedGameTime + (4 * m_GameTPS);
		m_mainStateMachine++;
		break;
	case 2:
		if (m_ElapsedGameTime >= m_mainWaitTillTime)
			m_mainStateMachine++;
		break;
	case 3:
		AudioMessage("edf01_01.wav");
		Goto(Object_APC1,"APCDest",1);
		Goto(Object_APC2,"APCDest",1);
		m_mainWaitTillTime = m_ElapsedGameTime + (17 * m_GameTPS);
		m_mainStateMachine++;
	case 4:
		if (m_ElapsedGameTime >= m_mainWaitTillTime)
			m_mainStateMachine++;
		break;
	case 5:
		EjectPilot(Object_TempCreation);
		m_mainWaitTillTime = m_ElapsedGameTime + (3 * m_GameTPS);
		m_mainStateMachine++;
	case 6:
		if (m_ElapsedGameTime >= m_mainWaitTillTime)
			m_mainStateMachine++;
		break;
	case 7:
		AudioMessage("edf01_02.wav");
		//RunSpeed,_Routine1,0,true,true
		AddObjective(_Text1,WHITE);
		AudioMessage("edf01_03.wav");
		m_mainStateMachine++;
		break;
	case 8: // move to target for scanning
		{
			sprintf_s(TempMsgString, "Nav%iA", Variable_PowerSourceNumber);
			Goto(Object_Buddy1,TempMsgString,1);
			Goto(Object_Buddy3,TempMsgString,1);
			Goto(Object_Buddy5,TempMsgString,1);

			sprintf_s(TempMsgString, "Nav%iB", Variable_PowerSourceNumber);
			Goto(Object_Buddy2,TempMsgString,1);
			Goto(Object_Buddy4,TempMsgString,1);
			Goto(Object_Buddy6,TempMsgString,1);

			sprintf_s(TempMsgString, "PowerSource%i", Variable_PowerSourceNumber);
			Object_CurrentPowerSource = GetHandle(TempMsgString);
			SetObjectiveOn(Object_CurrentPowerSource);

			m_mainStateMachine++;
		}
	case 9: // do scanning
		{
			sprintf_s(TempMsgString, "PowerSource%i", Variable_PowerSourceNumber);
			Object_CurrentPowerSource = GetHandle(TempMsgString);
			sprintf_s(TempMsgString,"Power source: %i%% scanned",(Variable_ScannedPercent/10));
			SetObjectiveName(Object_CurrentPowerSource,TempMsgString);

			Object_Player = GetPlayerHandle();
			Dist dist = GetDistance(Object_CurrentPowerSource,Object_Player);
			if (dist <= 190)
			{
				Variable_ScannedPercent += (40 / m_GameTPS);//+= 4;
				sprintf_s(TempMsgString,"Power source: %i%% scanned",(Variable_ScannedPercent/10));
				SetObjectiveName(Object_CurrentPowerSource,TempMsgString);

				if(Variable_ScannedPercent >= 1000)
				{
					SetObjectiveOff(Object_CurrentPowerSource);
					Variable_PowerSourceNumber++;
					Variable_ScannedPercent = 0;
					if(Variable_PowerSourceNumber <= 1)
					{
						ClearObjectives();
						AddObjective(_Text5,WHITE);
						m_mainStateMachine = 8;
					}else{
						m_mainWaitTillTime = m_ElapsedGameTime + (61 * m_GameTPS);
						m_mainStateMachine++;
					}
				}
			}
		}
		break;
	case 10:
		if (m_ElapsedGameTime >= m_mainWaitTillTime)
			m_mainStateMachine++;
		break;
	case 11:
		Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn1");
		Patrol(Object_TempCreation,"EnemyPatrol1",0);
		Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn1");
		Patrol(Object_TempCreation,"EnemyPatrol1",0);
		m_mainWaitTillTime = m_ElapsedGameTime + (67 * m_GameTPS);
		m_mainStateMachine++;
	case 12:
		if (m_ElapsedGameTime >= m_mainWaitTillTime)
			m_mainStateMachine++;
		break;
	case 13:
		Object_TempCreation = BuildObject("evtank",5,"AttackerSpawn1");
		Patrol(Object_TempCreation,"EnemyPatrol1",0);
		m_mainWaitTillTime = m_ElapsedGameTime + (61 * m_GameTPS);
		m_mainStateMachine++;
	case 14:
		if (m_ElapsedGameTime >= m_mainWaitTillTime)
			m_mainStateMachine++;
		break;
	case 15:
		Object_TempCreation = BuildObject("evtank",5,"AttackerSpawn1");
		Patrol(Object_TempCreation,"EnemyPatrol1",0);
		m_mainStateMachine++;
		break;
	case 16:
		m_SpawnAttackerLoop1 = true;
		if(Variable_SurvivorSafeCount > 0)
		{
			m_SpawnAttackerLoop1 = false;
			m_SpawnAttackerLoop2 = true;
			m_mainStateMachine++;
		}
		break;
	}

	if(m_SpawnAttackerLoop1)
	{
		switch(m_SpawnAttackerLoopStateMachine)
		{
		case 0:
			m_SpawnAttackerLoopWaitTillTime = m_ElapsedGameTime + (75 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine++;
		case 1:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime)
				m_SpawnAttackerLoopStateMachine++;
			break;
		case 2:
			Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime = m_ElapsedGameTime + (95 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine++;
		case 3:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime)
				m_SpawnAttackerLoopStateMachine++;
			break;
		case 4:
			Object_TempCreation = BuildObject("evtank",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime = m_ElapsedGameTime + (75 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine++;
		case 5:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime)
				m_SpawnAttackerLoopStateMachine++;
			break;
		case 6:
			Object_TempCreation = BuildObject("evtank",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopStateMachine = 0;
			break;
		}
	}

	if(m_SpawnAttackerLoop2)
	{
		switch(m_SpawnAttackerLoopStateMachine2)
		{
		case 0:
			m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine2++;
		case 1:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 2:
			Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (95 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine2++;
		case 3:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 4:
			Object_TempCreation = BuildObject("evtank",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine2++;
		case 5:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 6:
			Object_TempCreation = BuildObject("evtank",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine2++;
		case 7:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 8:
			Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			Object_TempCreation = BuildObject("evscout",5,"AttackerSpawn2");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (95 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine2++;
		case 9:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 10:
			Object_TempCreation = BuildObject("evtank",5,"TurretSpawn");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (75 * m_GameTPS);
			m_SpawnAttackerLoopStateMachine2++;
		case 11:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 12:
			Object_TempCreation = BuildObject("evtank",5,"TurretSpawn");
			Patrol(Object_TempCreation,"EnemyPatrol1",0);
			if(Variable11 < 15)
			{
				m_SpawnAttackerLoopStateMachine2 = 0;
			}else{
				m_SpawnAttackerLoopWaitTillTime2 = m_ElapsedGameTime + (140 * m_GameTPS);
				m_SpawnAttackerLoopStateMachine2++;
			}
		case 13:
			if (m_ElapsedGameTime >= m_SpawnAttackerLoopWaitTillTime2)
				m_SpawnAttackerLoopStateMachine2++;
			break;
		case 14:
			m_SpawnAttackerLoopStateMachine2 = 0;
		}
	}

/*
//--------------------------------------------------------------------
[routine,_Routine2,1,true]
	Wait,40
	Createp,Object_Observer1,"evtank",5,"Observer1"
	LookAt,Object_Observer1,Object_Player,0
	Createp,Object_Observer2,"evtank",5,"Observer2"
	LookAt,Object_Observer2,Object_Player,0
	Wait,10
	Remove,Object_APC1
	Remove,Object_APC2
LOC_136:
	GetTime
	IfGT,200,LOC_150
	Add,Variable_PowerSourceNumber,0
	IfLT,1,LOC_136
LOC_140:
	GetTime
	IfGT,200,LOC_150
	ShotBy,Object_TmpForChecks,Object_Observer1
	GetTeam,Object_TmpForChecks
	IfEQ,1,LOC_150
	ShotBy,Object_TmpForChecks,Object_Observer2
	GetTeam,Object_TmpForChecks
	IfEQ,1,LOC_150
	Add,Variable_ScannedPercent,0
	IfLT,45,LOC_140
LOC_150:
	Wait,4
	Audio,"edf01_04.wav"
	SetPTeam,Object_Player,1
	SetPTeam,Object_Buddy1,1
	SetPTeam,Object_Buddy2,1
	Attack,Object_Observer1,Object_Buddy1,0
	Attack,Object_Observer2,Object_Buddy2,0
	SetPTeam,Object_Buddy1,1
	SetPTeam,Object_Buddy2,1
	SetPTeam,Object_Buddy3,1
	SetPTeam,Object_Buddy4,1
	SetPTeam,Object_Buddy5,1
	SetPTeam,Object_Buddy6,1
	Clear
	Display,_Text2,white
	Wait,4
	SetGroup,Object_Buddy1,0
	Stop,Object_Buddy1,0
	SetGroup,Object_Buddy2,0
	Stop,Object_Buddy2,0
	SetGroup,Object_Buddy3,0
	Stop,Object_Buddy3,0
	SetGroup,Object_Buddy4,0
	Stop,Object_Buddy4,0
	SetGroup,Object_Buddy5,0
	Stop,Object_Buddy5,0
	SetGroup,Object_Buddy6,0
	Stop,Object_Buddy6,0
	BeaconOff,Object_CurrentPowerSource
LOC_179:
	HasPilot,Object_Observer1
	IfEQ,1,LOC_179
	GetTeam,Object_Observer1
	IfEQ,5,LOC_179
	HasPilot,Object_Observer2
	IfEQ,1,LOC_179
	GetTeam,Object_Observer2
	IfEQ,5,LOC_179
	SetStep,_Routine1,LOC_72
	RunSpeed,_Routine1,1,true,true
	RunSpeed,_Routine5,8,true
	RunSpeed,_Routine7,3,true
	RunSpeed,_Routine6,1,true
	RunSpeed,_Routine9,1,true
	Wait,7
	Audio,"edf01_05.wav"
	Wait,14
	Audio,"edf01_05A.wav"
	Createp,Object_SurvivorDropoff,"ibnav",1,"SafeNav"
	SetName,Object_SurvivorDropoff,"Survivor Dropoff"
	BeaconOn,Object_SurvivorDropoff
	Createp,Object_TmpForChecks,"ibnav",1,"SurvivorNav1"
	SetName,Object_TmpForChecks,"Survivors"
	BeaconOn,Object_TmpForChecks
	Createp,Object_TmpForChecks,"ibnav",1,"SurvivorNav2"
	SetName,Object_TmpForChecks,"Survivors"
	BeaconOn,Object_TmpForChecks
	Createp,Object_TmpForChecks,"evscout",5,"Enemy2"
	Createp,Object_TmpForChecks,"evscout",5,"Enemy3"
	Wait,140
LOC_209:
	GetPlayer,Object_Player
	DistObject,Object_DropShip,Object_Player
	IfLT,200,LOC_209
	Createp,Object_TmpForChecks,">evturr",5,"TurretSpawn"
	GoTo,Object_TmpForChecks,"Turret1",1
	Createp,Object_TmpForChecks,">evturr",5,"TurretSpawn"
	GoTo,Object_TmpForChecks,"Turret2",1
	Createp,Object_TmpForChecks,">evturr",5,"TurretSpawn"
	GoTo,Object_TmpForChecks,"Turret3",1
	Stop,Object_FriendTurret3,0
	Stop,Object_FriendTurret2,0
	Createp,Object_TmpForChecks,">ivserv",1,"Buddy4"
	SetGroup,Object_TmpForChecks,2
	Createp,Object_TmpForChecks,">ivserv",1,"Buddy5"
	SetGroup,Object_TmpForChecks,3
	Createp,Object_TmpForChecks,">ivserv",1,"Buddy6"
	SetGroup,Object_TmpForChecks,4
	Audio,"edf01_05B.wav"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
[routine,_Routine4,1,true]
	OnDelObject,100,_Routine4,Object_RemoveObjectCheck
LOC_261:
	RunSpeed,_Routine4,0,true
	IsODF,Object_RemoveObjectCheck,"evtank"
	IfEQ,1,LOC_266
	IsODF,Object_RemoveObjectCheck,"evscout"
	IfEQ,0,LOC_270
LOC_266:
	GetTeam,Object_RemoveObjectCheck
	IfNE,5,LOC_261
	Add,Variable11,-1,Variable11
	JumpTo,LOC_261
LOC_270:
	IsODF,Object_RemoveObjectCheck,">ivtank_e01"
	IfEQ,1,LOC_274
	IsODF,Object_RemoveObjectCheck,">ivtank"
	IfEQ,0,LOC_261
LOC_274:
	SetByIndex,Object31,Variable_SurvivorNumber4,Object_RemoveObjectCheck
	RunSpeed,_Routine8,100,true
	JumpTo,LOC_261

//--------------------------------------------------------------------
[routine,_Routine5,0,true]
LOC_278:
	GetByLabel,Object_TempSurvivor,"Survivor%i",Variable_SurvivorNumber
	SetByIndex,Object_SurvivorArray,Variable_SurvivorNumber,Object_TempSurvivor
	SetMaxHealth,Object_TempSurvivor,800
	SetCurHealth,Object_TempSurvivor,800
	Add,Variable_SurvivorNumber,1,Variable_SurvivorNumber
	IfLT,10,LOC_278
	Set,Variable_SurvivorNumber,0
LOC_285:
	RunSpeed,_Routine5,1,true
	RunSpeed,_Routine5,8,true
	GetByIndex,Object_TempSurvivor,Object_SurvivorArray,Variable_SurvivorNumber
	IsAround,Object_TempSurvivor
	IfEQ,1,LOC_310
	Add,Variable12,0
	IfGT,0,LOC_335
	Set,Variable12,1
	SetByIndex,Object_SurvivorArray,Variable_SurvivorNumber,Object_DropShip
	Add,Variable_SurvivorSafeCount,0
	IfLT,9,LOC_306
	RunSpeed,_Routine6,0,true
	Clear
	Display,_Text10,red
	Audio,"edf01_06a.wav"
	Wait,10
	Clear
	Display,_Text11,green
	Audio,"edf01_06.wav"
	Succeed,15,"edf01W2.txt"
	RunSpeed,_Routine5,0,true
LOC_306:
	Clear
	Display,_Text10,red
	Audio,"edf01_06a.wav"
	RunSpeed,_Routine9,1,true
LOC_310:
	IsODF,Object_TempSurvivor,"ispilo"
	IfEQ,0,LOC_331
	NearVehicle,Object_NearSurvivor,Object_TempSurvivor
	IsPlayer,Object_NearSurvivor
	IfEQ,1,LOC_331
	IsODF,Object_NearSurvivor,">ivtank"
	IfEQ,0,LOC_331
	DistObject,Object_NearSurvivor,Object_TempSurvivor
	IfGT,24,LOC_331
	Add,Variable10,1,Variable10
	Replace,Object_NearSurvivor,">ivtank_e01",1
	SetByIndex,Object_SurvivorArray,Variable_SurvivorNumber,Object_NearSurvivor
	Remove,Object_TempSurvivor
	Audio,"ivtank03.wav"
	Add,Variable_SurvivorNumber,1,Variable_SurvivorNumber6
	SetName,Object_NearSurvivor,"Has survivor %i",Variable_SurvivorNumber6
	BeaconOn,Object_NearSurvivor
	SetGroup,Object_NearSurvivor,9
	Wait,1
	GetPlayer,Object_Player
	Follow,Object_NearSurvivor,Object_Player,0
LOC_331:
	Add,Variable_SurvivorNumber,1,Variable_SurvivorNumber
	IfLT,10,LOC_285
	Set,Variable_SurvivorNumber,0
	JumpTo,LOC_285
LOC_335:
	Clear
	Display,_Text7,red
	Audio,"edf01_07.wav"
	Fail,15,"edf01L2.txt"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
[routine,_Routine7,0,true]
LOC_377:
	GetByIndex,Object_SurvivorForPilotCheck,Object_SurvivorArray,Variable_SurvivorNumber3
	IsODF,Object_SurvivorForPilotCheck,"ispilo"
	IfEQ,0,LOC_382
	Add,Variable_SurvivorNumber5,1,Variable_SurvivorNumber5
	SetByIndex,Object_WaitingSurvivor,Variable_SurvivorNumber4,Object_SurvivorForPilotCheck
LOC_382:
	Add,Variable_SurvivorNumber3,1,Variable_SurvivorNumber3
	IfLT,10,LOC_377
	Add,Variable_SurvivorNumber5,0
	IfEQ,0,LOC_395
	IfEQ,1,LOC_389
	SetName,Object_WaitingSurvivor,"%i Survivors Waiting",Variable_SurvivorNumber5
	JumpTo,LOC_390
LOC_389:
	SetName,Object_WaitingSurvivor,"Last Waiting Survivor"
LOC_390:
	BeaconOn,Object_WaitingSurvivor
	Wait,2
	Set,Variable_SurvivorNumber3,0
	Set,Variable_SurvivorNumber5,0
	JumpTo,LOC_377
LOC_395:
//--------------------------------------------------------------------
//--------------------------------------------------------------------
[routine,_Routine9,1,true]
LOC_409:
	RunSpeed,_Routine9,0,true
	Wait,15
	RunSpeed,_Routine9,5,true
	Clear
	Display,_Text3,white
	Display,_Text4,white
	JumpTo,LOC_409
*/


	if((m_mainStateMachineLast != m_mainStateMachine)
		|| (m_SpawnAttackerLoopStateMachineLast != m_SpawnAttackerLoopStateMachine)
		|| (m_SpawnAttackerLoopStateMachine2Last != m_SpawnAttackerLoopStateMachine2))
	{
		sprintf_s(TempMsgString, "Main: %d, Attack1: %d, Attack2: %d"
			, m_mainStateMachine
			, m_SpawnAttackerLoopStateMachine
			, m_SpawnAttackerLoopStateMachine2);
		PrintConsoleMessage(TempMsgString);
		m_mainStateMachineLast = m_mainStateMachine;
		m_SpawnAttackerLoopStateMachineLast = m_SpawnAttackerLoopStateMachine;
		m_SpawnAttackerLoopStateMachine2Last = m_SpawnAttackerLoopStateMachine2;
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

DLLBase * BuildMission(void)
{
	return new EDF01Mission();
}
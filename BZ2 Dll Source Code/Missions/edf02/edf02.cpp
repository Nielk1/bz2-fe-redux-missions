#include "edf02.h"

char* OBJECTIVE_TEXT_1 = "Scout ahead of the Recycler and\nmake sure this canyon is safe!";
char* OBJECTIVE_TEXT_2 = "You've lost the Recycler and\na platoon's worth of good men.\nWe now have no hope of securing\nour eastern flank.";
char* OBJECTIVE_TEXT_3 = "The Hadeans are alerted to our\npresence. You must act quickly to\n make it to safer ground!";
char* OBJECTIVE_TEXT_4 = "Deploy the Recycler, and get\nsome gun towers built. Hurry--you\ndon't have much time!";
char* OBJECTIVE_TEXT_5 = "Excellent job, Lieutenant! The\nGeneral will be pleased to hear\nthat we have a competent CO \nholding our eastern flank.\nReinforcements are en route.";
char* OBJECTIVE_TEXT_6 = "Command has some suspicions.\nStay clear and observe the\nactivity near the structure.";
char* OBJECTIVE_TEXT_7 = "Get your units out of the\ncanyon through that portal.\nIt may be your only hope.";
char* OBJECTIVE_TEXT_8 = "Use the 'I' key of your Digital\nControl Interface to scan the\nstructure.";
char* OBJECTIVE_TEXT_9 = "Try to pass through the portal\nby cruising under the arch. If\nyou succeed, try to return in the\nsame way.";
char* OBJECTIVE_TEXT_10 = "Sensors show a major attack wave\napproaching from two directions\nand closing fast!";
char* OBJECTIVE_TEXT_11 = "The Hadean attackers are almost\nwithin firing range!  Brace\n for an attack!";
char* OBJECTIVE_TEXT_12 = "You cannot seem to follow a\nsimple order, Lieutenant. Return\nto base and turn in your sidearm\nand wings.";

#define AUDIO_01 "edf02_01.wav"
#define AUDIO_02 "edf02_02.wav"
#define AUDIO_03 "edf02_03.wav"
#define AUDIO_04 "edf02_04.wav"
#define AUDIO_05 "edf02_05.wav"
#define AUDIO_05A "edf02_05A.wav"
#define AUDIO_06 "edf02_06.wav"
#define AUDIO_07 "edf02_07.wav"
#define AUDIO_08 "edf02_08.wav"
#define AUDIO_09 "edf02_09.wav"
#define AUDIO_10 "edf02_10.wav"
#define AUDIO_11 "edf02_11.wav"
#define AUDIO_12 "edf02_12.wav"

#define Variable_FlyingDropshipTurnSpeed 0
#define Variable_FlyingDropshipMoveSpeed 300

#define Position_FlyingDropshipStartAt Vector(-450,-120,-1460)
#define Position_FlyingDropshipMoveTo Vector(-590,-155,-1190)
#define Position_DeployBase Vector(420,0,920)

void EDF02Mission::Init(void)
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

	SetAutoGroupUnits(false);

	m_DidOneTimeInit = true;

	m_Routine3Active = true;

	Preload();

	//m_Gravity = GetVarItemInt("network.session.ivar31");
	// Set this for the server now. Clients get this set from Load().
	//SetGravity(static_cast<float>(m_Gravity) * 0.5f);
}

void EDF02Mission::AddObject(Handle h)
{
//	char ODFName[64];
//	GetObjInfo(h, Get_CFG, ODFName);

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

	if(IsOdf(h,"ibgtow"))
	{
		Variable_CountGuntowers++;
	}else if(IsOdf(h,"ibrecy"))
	{
		RemoveObject(Object_MissionNav);
	}
}

void EDF02Mission::DeleteObject(Handle h)
{

}

void EDF02Mission::Preload()
{

}

bool EDF02Mission::Load(bool missionSave)
{
	SetAutoGroupUnits(false);

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

		RebuildMission::Load(missionSave);

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

	ret = ret && RebuildMission::Load(missionSave);

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


bool EDF02Mission::PostLoad(bool missionSave)
{
	if (missionSave)
	{
		RebuildMission::PostLoad(missionSave);
		return true;
	}

	bool ret = true;

	ret = ret && RebuildMission::PostLoad(missionSave);
	ConvertHandles(h_array, h_count);

	//ret = ret && PUPMgr::PostLoad(missionSave);
	return ret;
}


bool EDF02Mission::Save(bool missionSave)
{
	if (missionSave)
	{
		RebuildMission::Save(missionSave);
		return true;
	}

	bool ret = true;

	ret = ret && RebuildMission::Save(missionSave);

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

void EDF02Mission::Execute(void)
{
	RebuildMission::Execute();

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

	//sprintf(TempMsgString,"Routine 1 waiting at step %i %i / %i",m_Routine1,m_Routine1WaitTillTime,m_ElapsedGameTime);
	//PrintConsoleMessage(TempMsgString);

	if (m_ElapsedGameTime >= m_Routine1WaitTillTime)
	{
		switch(m_Routine1)
		{
		case 0:
			StartEarthQuake(10.00f);
			Object_FlyingDropship = BuildObject("ivdrop_fly", 0, Position_FlyingDropshipMoveTo);
			SetVectorPosition(Object_FlyingDropship,Position_FlyingDropshipStartAt);
			Wait(m_Routine1WaitTillTime, 7);
			m_Routine1++;
			break;
		case 1:
			AudioMessage(AUDIO_01);
			Wait(m_Routine1WaitTillTime, 3);
			m_Routine1++;
			break;
		case 2:
			if(CameraReady())
			{
				m_Routine1++;
			}else{
				break;
			}
		case 3:
			CameraPath("CamPath",5500,3200 * (60 / m_GameTPS),Object_FlyingDropship);
			if(!Move(Object_FlyingDropship,Variable_FlyingDropshipTurnSpeed,Variable_FlyingDropshipMoveSpeed,Position_FlyingDropshipMoveTo))
			{
				m_Routine1++;
			}else{
				break;
			}
		case 4:
			CameraFinish();
			RemoveObject(Object_FlyingDropship);
			AudioMessage("xemt2.wav");
			Object_Player = GetPlayerHandle();
			StopEarthQuake();
			SetPosition(Object_Player,"PlacePlayer");
			SetVelocity(Object_Player,Vector(0,0,40));
			Wait(m_Routine1WaitTillTime, 3);

			//m_Routine2 = 5; // start the old seperate logic for the camera, it does dinos too
			m_Routine2Active = true;

			m_Routine1++;
			break;
		case 5:
			AudioMessage(AUDIO_02);
			Goto(Object_Recycler,"RecyclerPath",1);
			Object_MissionNav = BuildObject("ibnav",1,"NavSpawn");
			SetObjectiveName(Object_MissionNav,"Investigate");
			SetObjectiveOn(Object_MissionNav);
			AddObjective(OBJECTIVE_TEXT_1, WHITE);
			Object_Portal1 = GetHandle("Portal1");
			SetPortal(Object_Portal1);
			Object_Portal2 = GetHandle("Portal2");
			SetPortal(Object_Portal2);
			SetPortalDest(Object_Portal2,Object_Portal1);
			Object_Portal3 = GetHandle("Portal3");
			SetPortal(Object_Portal3);
			SetPortalDest(Object_Portal3,Object_Portal1);
			Object_Portal4 = GetHandle("Portal4");
			SetPortal(Object_Portal4);
			SetPortalDest(Object_Portal4,Object_Portal1);
			Object_Portal5 = GetHandle("Portal5");
			SetPortal(Object_Portal5);
			SetPortalDest(Object_Portal5,Object_Portal1);
			Object_Pool1 = GetHandle("Pool1");
			m_Routine4Active = true; //RunSpeed,_Routine4,1,false
			Wait(m_Routine1WaitTillTime, 30);
			m_Routine1++;
			break;
		case 6:
			AudioMessage(AUDIO_03);
			Wait(m_Routine1WaitTillTime, 220);
			m_Routine1++;
			break;
		case 7:
			AudioMessage(AUDIO_04);
			Wait(m_Routine1WaitTillTime, 5);
			m_Routine1++;
			break;
		case 8:
			AddObjective(OBJECTIVE_TEXT_3, WHITE);
			Object_TmpForSpawning = BuildObject("evscout_e02",5,"Spawn2");
			Patrol(Object_TmpForSpawning,"Patrol2",0);
			m_Routine1++;
			break;
		case 9:
			if(GetTime() < 780)
			{
				Object_TmpForSpawning = BuildObject("evscout_e02",5,"Spawn1");
				Goto(Object_TmpForSpawning,"Patrol1",0);
				Wait(m_Routine1WaitTillTime, 48);
			}else{
				m_Routine1++;
			}
			break;
		case 10:
			Object_TmpForSpawning = BuildObject("evtank",5,"Spawn1");
			Goto(Object_TmpForSpawning,"Patrol1",0);
			Wait(m_Routine1WaitTillTime, 27);
			m_Routine1++;
			break;
		case 11:
			Object_TmpForSpawning = BuildObject("evscout_e02",5,"Spawn1");
			Goto(Object_TmpForSpawning,"Patrol1",0);
			Wait(m_Routine1WaitTillTime, 48);
			m_Routine1 = 10;
			break;
		}
	}

	if(m_Routine2Active)
	{
		if (m_ElapsedGameTime >= m_Routine2WaitTillTime)
		{
			switch(m_Routine2)
			{
			case 0:
				Wait(m_Routine2WaitTillTime,100);
				m_Routine2++;
				break;
			case 1: //LOC_57:
				Variable_GameTime_DinoSpawns = GetTime() + 250;
				m_Routine2++;
			case 2: //LOC_59:
				if(!IsAround(Object_DinoB))
				{
					Object_DinoB = BuildObject("raptor01",3,"DinoSpawn1");
					Goto(Object_DinoB,"DinoPatrol1",0);
				}
				m_Routine2++;
			case 3: //LOC_63:
				if(!IsAround(Object_DinoA))
				{
					Object_DinoA = BuildObject("raptor01",3,"DinoSpawn1");
					Goto(Object_DinoA,"DinoPatrol1",0);
				}
				m_Routine2++;
			case 4: //LOC_67:
				if(GetTime() < Variable_GameTime_DinoSpawns) // keep respawning the dinos if needed and the time is not too past
				{
					m_Routine2 = 2; //GOTO LOC_59;
				}else{
					m_Routine2++;
				}
				break;
			case 5:
				Goto(Object_DinoB,"DinoPatrol1",0);
				Goto(Object_DinoA,"DinoPatrol1",0);
				m_Routine2 = 1; //JumpTo,LOC_57
				break;
			}
		}
	}

	if(m_Routine3Active)
	{
		if (m_ElapsedGameTime >= m_Routine3WaitTillTime)
		{
			switch(m_Routine3)
			{
			case 0:
				Object_Player = GetPlayerHandle();
				GetPosition(Object_Player,Position_PlayerPosition);
				Object_StayPut = BuildObject("stayput",0,Position_PlayerPosition);
				//m_Routine3Active = true; //RunSpeed,_Routine3,1,true
				Wait(m_Routine3WaitTillTime,12);
				m_Routine3++;
				break;
			case 1:
				RemoveObject(Object_StayPut);
				m_Routine3++;
			case 2: //LOC_79:
				if(IsOdf(Object_PossibleObjectNearPortal,"evscav"))
				{
					m_Routine3++;
					Wait(m_Routine3WaitTillTime, 3);
				}
				break;
			case 3:
				AudioMessage(AUDIO_05);
				Wait(m_Routine3WaitTillTime,7);
				m_Routine3++;
				break;
			case 4:
				ReplaceObject(Object_ObservedPortalUser1,"evscav",1);
				SetObjectiveName(Object_ObservedPortalUser1,"Teleported Harvester");
				Stop(Object_ObservedPortalUser1,0);
				m_Routine8 = 1;//SetStep,_Routine8,LOC_259
				Wait(m_Routine3WaitTillTime,900);
				m_Routine3++;
				break;
			case 5:
				ClearObjectives();
				AudioMessage(AUDIO_08);
				AddObjective(OBJECTIVE_TEXT_10,WHITE);
				Variable_GameTime_MakeGuntowersFastEnough = GetTime() + 600;
				m_Routine3++;
			case 6: //LOC_94:
				if(GetTime() > Variable_GameTime_MakeGuntowersFastEnough)
				{
					m_Routine3 = 8;//JUMPTO LOC_104
				}else if(Variable_CountGuntowers > 0)
				{
					Variable_GameTime_MakeGuntowersFastEnough = GetTime() + 230;
					m_Routine3++;
				}
				break;
			case 7: //LOC_100:
				if(Variable_CountGuntowers > 1)
				{
					m_Routine3++; // JUMPTO LOC_104
				}else if(GetTime() >= Variable_GameTime_MakeGuntowersFastEnough)
				{
					m_Routine3++;
				}
				break;
			case 8: //LOC_104:
				Wait(m_Routine3WaitTillTime,50);
				m_Routine3++;
				break;
			case 9:
				AudioMessage(AUDIO_09);
				AddObjective(OBJECTIVE_TEXT_1,WHITE);
				m_Routine3++;
			case 10: //LOC_107:
				//RunSpeed,_Routine1,1,true
				Object_Wave_Attackers[0] = BuildObject("evtank",5,"Wave1");
				Attack(Object_Wave_Attackers[0],Object_Recycler,0);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 11:
				Object_Wave_Attackers[1] = BuildObject("evtanku",5,"Wave1");
				Goto(Object_Wave_Attackers[1],Object_Recycler,1);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 12:
				Object_Wave_Attackers[2] = BuildObject("evscout_e02",5,"Wave1");
				Goto(Object_Wave_Attackers[2],Object_Recycler,1);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 13:
				Object_Wave_Attackers[3] = BuildObject("evmislu",5,"Wave1");
				Attack(Object_Wave_Attackers[3],Object_Recycler,0);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 14:
				Object_Wave_Attackers[4] = BuildObject("evtanku",5,"Wave2");
				Attack(Object_Wave_Attackers[4],Object_Recycler,0);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 15:
				Object_Wave_Attackers[5] = BuildObject("evmislu",5,"Wave2");
				Goto(Object_Wave_Attackers[5],Object_Recycler,1);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 16:
				Object_Wave_Attackers[6] = BuildObject("evscout_e02",5,"Wave2");
				Goto(Object_Wave_Attackers[6],Object_Recycler,1);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 17:
				Object_Wave_Attackers[7] = BuildObject("evmort",5,"Wave2");
				SetMaxHealth(Object_Wave_Attackers[7],2800);
				SetCurHealth(Object_Wave_Attackers[7],2800);
				Goto(Object_Wave_Attackers[7],Object_Recycler,1);
				Wait(m_Routine3WaitTillTime,3);
				m_Routine3++;
				break;
			case 18:
				m_Routine4Active = true; //RunSpeed,_Routine1,4,true
				{
					bool waveDefeated = true;
					for(int x=0;x<8;x++)
					{
						if(IsAliveAndPilot(Object_Wave_Attackers[x])
						&& (GetTeamNum(Object_Wave_Attackers[x]) == 5))
						{
							waveDefeated = false;
							break;
						}
					}
					if(waveDefeated)
					{
						Variable_DefeatedAttackWaveCount++;
						if(Variable_DefeatedAttackWaveCount < 3)
						{
							m_Routine3 = 10;//JUMPTO LOC_107
						}else{
							m_Routine3++;
						}
					}
				}
				break;
			case 19:
				AudioMessage(AUDIO_10);
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_5,GREEN);
				SucceedMission(GetTime() + 16.0f,"edf02W1.txt");
				m_Routine3++;
				break;
			}
		}
	}

	if(m_Routine4Active)
	{
		//[routine,_Routine4,0,false]
		if (m_ElapsedGameTime >= m_Routine4WaitTillTime)
		{
			switch(m_Routine4)
			{
			case 0: //LOC_153:
				Object_Player = GetPlayerHandle();
				if(GetDistance(Object_Player,Object_MissionNav) <= 220)
				{
					m_Routine4++;
				}
				break;
			case 1:
				SetObjectiveOff(Object_MissionNav);
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_6,WHITE);
				Object_ObservedPortalUser1 = BuildObject("evscav",5,"EnemyScav");
				SetObjectiveName(Object_ObservedPortalUser1,"Observe");
				SetObjectiveOn(Object_ObservedPortalUser1);
				Goto(Object_ObservedPortalUser1,Object_Portal1,1);
				Wait(m_Routine4WaitTillTime,25);
				m_Routine4++;
				break;
			case 2:
				Object_ObservedPortalUser2 = BuildObject("evscout",5,"Spawn2");
				Goto(Object_ObservedPortalUser2,Object_Portal1,0);
				Wait(m_Routine4WaitTillTime,6);
				m_Routine4++;
				break;
			case 3:
				SetObjectiveOn(Object_ObservedPortalUser2);
				m_Routine4++;
				break;
			}
		}
	}

	//if(m_Routine5Active)
	{
		//[routine,_Routine5,1,false]
		if (m_ElapsedGameTime >= m_Routine5WaitTillTime)
		{
			switch(m_Routine5)
			{
			case 0:
				Object_Recycler = GetHandle("Recycler");
				Stop(Object_Recycler,1);
				Wait(m_Routine5WaitTillTime,10);
				m_Routine5++;
				break;
			case 1:
				Object_DeployingDropship = GetHandle("DropShip");
				SetAnimation(Object_DeployingDropship,"Deploy",1);
				SetPosition(Object_Recycler,"RecyclerPath");
				SetVelocity(Object_Recycler,Vector(0,0,15));
				Object_TmpForSpawning = BuildObject("ivtank",1,"Escort1");
				SetGroup(Object_TmpForSpawning,2);
				Follow(Object_TmpForSpawning,Object_Recycler,0);
				Object_TmpForSpawning = BuildObject("ivscout",1,"Escort2");
				SetGroup(Object_TmpForSpawning,1);
				Follow(Object_TmpForSpawning,Object_Recycler,0);
				Object_TmpForSpawning = BuildObject("ivscout",1,"Escort3");
				SetGroup(Object_TmpForSpawning,1);
				Follow(Object_TmpForSpawning,Object_Recycler,0);
				Object_TmpForSpawning = BuildObject("ivscout",1,"Escort4");
				SetGroup(Object_TmpForSpawning,1);
				Follow(Object_TmpForSpawning,Object_Recycler,0);
				Object_TmpForSpawning = BuildObject("evscout_e02",5,"Enemy1");
				SetSkill(Object_TmpForSpawning,3);
				Object_TmpForSpawning = BuildObject("evscout_e02",5,"Enemy2");
				SetSkill(Object_TmpForSpawning,3);
				SetGroup(Object_Recycler,0);
				SetScrap(1,30);
				m_Routine5++;
			case 2: //LOC_194:
				if(!IsAround(Object_Recycler))
				{
					m_Routine5++;
				}
				break;
			case 3:
				m_Routine3Active = false;//RunSpeed,_Routine3,0,true
				AudioMessage(AUDIO_11);
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_2,RED);
				FailMission(GetTime() + 12.0f,"edf02L1.txt");
				m_Routine5++;
				break;
			}
		}
	}

	if(m_Routine6Active)
	{
		if (m_ElapsedGameTime >= m_Routine6WaitTillTime)
		{
			switch(m_Routine6)
			{
			case 0:
				Wait(m_Routine6WaitTillTime,3);
				m_Routine6++;
				break;
			case 1:
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_4,WHITE);
				Variable_Bool_StartDeployBase = true;
				Goto(Object_Recycler,Object_MissionNav,0);
				m_Routine6++;
				break;
			}
		}
	}


	//[routine,_Routine8,1,true]
	if (m_ElapsedGameTime >= m_Routine8WaitTillTime)
	{
		switch(m_Routine8)
		{
		case 0: //LOC_256:
			if(IsPlayer(Object_PossibleObjectNearPortal)) // object near is player, 
			{
				m_Routine8 = 5; //JumpTo,LOC_288
			}
			break;
		case 1: //LOC_259:
			AudioMessage(AUDIO_05A);
			Variable_GameTime_UseThatPortal = GetTime() + 30;
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_9,WHITE);
			m_Routine8++;
		case 2: //LOC_264:
			if(IsPlayer(Object_PossibleObjectNearPortal))
			{
				m_Routine8 = 5; //JUMPTO LOC_288
			}else if(GetTime() >= Variable_GameTime_UseThatPortal)
			{
				AudioMessage(AUDIO_05A);
				Variable_GameTime_UseThatPortal = GetTime() + 30;
				m_Routine8++;
			}
			break;
		case 3: //LOC_271:
			if(IsPlayer(Object_PossibleObjectNearPortal))
			{
				m_Routine8 = 5; //JUMPTO LOC_288
			}else if(GetTime() >= Variable_GameTime_UseThatPortal)
			{
				AudioMessage(AUDIO_05A);
				Variable_GameTime_UseThatPortal = GetTime() + 30;
				m_Routine8++;
			}
			break;
		case 4: //LOC_278:
			if(IsPlayer(Object_PossibleObjectNearPortal))
			{
				m_Routine8 = 5; //JUMPTO LOC_288
			}else if(GetTime() >= Variable_GameTime_UseThatPortal)
			{
				ClearObjectives();
				AddObjective(OBJECTIVE_TEXT_12,RED);
				AudioMessage(AUDIO_12);
				FailMission(GetTime() + 12.0f,"EDF02_L2.txt");
				m_Routine3Active = false;//RunSpeed,_Routine3,0,true
				m_Routine8 = 13;//JumpTo,LOC_303
			}
			break;
		case 5: //LOC_288: // player used the portal
			if(Variable_Bool_StartDeployBase)
			{
				m_Routine8 = 10; //JUMPTO LOC_297
				break;
			}
			m_Routine8++;
		case 6:
			Wait(m_Routine8WaitTillTime,3);
			m_Routine8++;
			break;
		case 7:
			AudioMessage(AUDIO_06);
			Wait(m_Routine8WaitTillTime,3);
			m_Routine8++;
			break;
		case 8:
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_7,WHITE);
			m_Routine8++;
		case 9: //LOC_295:
			if(Variable_Bool_StartDeployBase)
			{
				m_Routine8++;
			}
			break;
		case 10: //LOC_297: // vase deployed
			{
				Object_Player = GetPlayerHandle();
				if(GetDistance(Object_Recycler,Object_Player) <= 150)
				{
					m_Routine8++;
				}
			}
			break;
		case 11:
			SetObjectiveOn(Object_Pool1);
			Wait(m_Routine8WaitTillTime,5);
			m_Routine8++;
			break;
		case 12:
			AudioMessage(AUDIO_07);
			m_Routine8++;
			break;
		//LOC_303:
		}
	}



	m_ElapsedGameTime++;
}

void EDF02Mission::OnPortalProximity(Handle Portal, Handle NearVehicle)
{
	//[routine,_Routine6,1,true]
	if(IsWithin(NearVehicle,Portal,15.0f))//OnPortalDist,60,_Routine6,Object_PortalForOnPortalDist,15
	{
		Object_PossibleObjectNearPortal = NearVehicle;
		Handle Object_PortalDestination = GetPortalDest(Portal);
		if(IsOdf(NearVehicle,"ivrecy") && !Variable_Bool_StartDeployBase)
		{
			SetObjectiveOff(Object_Portal1);
			Object_MissionNav = BuildObject("ibnav",1,Position_DeployBase);
			SetObjectiveName(Object_MissionNav,"Deploy Base");
			SetObjectiveOn(Object_MissionNav);
			Teleport(NearVehicle,Object_PortalDestination,30);
			//RunSpeed,_Routine6,1,true
			m_Routine6Active = true; // recycler just ported
		}else{
			if(Object_Portal1 != Portal)
			{
				if(GetTeamNum(NearVehicle) != 5)
				{
					Teleport(NearVehicle,Object_Portal1,30);
					if(!IsPlayer(NearVehicle)) Goto(NearVehicle,Object_Portal1,0);
				}

			}else{
				PrintConsoleMessage("From Portal 1");
				if(IsPlayer(NearVehicle))
				{
					//JUMPTO LOC_230
					Teleport(NearVehicle,Object_Portal4,30);
					//JumpTo,LOC_233
				}else{
					if(RandomInt(2) == 0)
					{
						Teleport(NearVehicle,Object_Portal3,30);
						//JumpTo,LOC_233
					}else{
						Teleport(NearVehicle,Object_Portal5,30);
						//JumpTo,LOC_233
					}
				}
				//LOC_233
				if(GetTeamNum(NearVehicle) == 5)
				{
					Goto(NearVehicle,Object_Recycler,0);
				}
			}
		}
	}
}

DLLBase * BuildMission(void)
{
	return new EDF02Mission();
}
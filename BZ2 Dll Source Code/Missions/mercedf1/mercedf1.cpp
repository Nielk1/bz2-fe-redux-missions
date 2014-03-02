#include "mercedf1.h"

char* OBJECTIVE_TEXT_1 = "Protect your assigned transport,\nhighlighted with a blue beacon.\nIt's carrying biometal-refining\nmodules for our Recycler, so\ndon't lose it!";
char* OBJECTIVE_TEXT_2 = "Follow Major Wyndt-Essex. Stay\nin loose formation, and cover\nher as necessary.";
char* OBJECTIVE_TEXT_3 = "Take out the gun tower near\nthe bridge. Major Wyndt-Essex\nwill stay out of range and watch\nfor enemy patrols.";
char* OBJECTIVE_TEXT_4 = "Now, take out the power supply.\nAnd hurry up; the Major says you\nare taking far too long!";
char* OBJECTIVE_TEXT_5 = "ATTENTION ALL UNITS: Begin to\nboard the StormPetrel in an\norderly fashion.";
char* OBJECTIVE_TEXT_6 = "Follow Hardin and head over to\nthe Training Center at the base.\nHop out of your vehicle and join\nthe General inside.";
char* OBJECTIVE_TEXT_7 = "Get into the StormPetrel.";
char* OBJECTIVE_TEXT_8 = "Your negligence and cowardice\nhave led to the death of Major\nWyndt-Essex.";
char* OBJECTIVE_TEXT_9 = "General Hardin is dead, and\nwith him all our hopes for \ndefending Earth from the Hadean\ninvasion.";
char* OBJECTIVE_TEXT_10 = "You failed to protect the\ntransport as directed. Without\nthe biometal-refining modules,\nour mission cannot continue.\nPrepare to head back to Earth.";
char* OBJECTIVE_TEXT_11 = "Defend the transports in the\nconvoy. Each one is carrying\ncritical components necessary\nfor our mission's success.";
char* OBJECTIVE_TEXT_12 = "haha, you lazy mpi players\njust plain suck. LOL";

#define ODF_ENEMY_DRONE "nadir"
#define ODF_FRIEND_SCOUT "ivscout11"
#define ODF_FRIEND_SERV "ivserv"
#define ODF_FRIEND_CARGO "ivcargo"

void EDF00Mission::Init(void)
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
	//Position_LandingZone1 = Vector(-674.0f,1000.0f,-783.0f);
	//Position_CarrierLaunchCamDummy = Vector(-674.0f,80.0f,-782.0f);
	//m_Gravity = GetVarItemInt("network.session.ivar31");
	// Set this for the server now. Clients get this set from Load().
	//SetGravity(static_cast<float>(m_Gravity) * 0.5f);

	Object_Power1 = GetHandle("power1");
	Object_Power2 = GetHandle("power2");
	Object_Power3 = GetHandle("power3");
	Object_Power4 = GetHandle("power4");
	Object_Power5 = GetHandle("power5");
	Object_Power6 = GetHandle("power6");
	Object_Power7 = GetHandle("power7");
	Object_Radar1 = GetHandle("Radar1");
	Object_Radar2 = GetHandle("Radar2");
	Object_Gun1 = GetHandle("gun1");
	Object_Gun2 = GetHandle("gun2");
	Object_Gun3 = GetHandle("gun3");
	Object_Gun4 = GetHandle("gun4");
	Object_Gun5 = GetHandle("gun5");
	Object_Gun6 = GetHandle("gun6");
	Object_Gun7 = GetHandle("gun7");
	Object_Gun8 = GetHandle("gun8");
	Object_Gun9 = GetHandle("gun9");
	Object_Gun10 = GetHandle("gun10");
	
	Object_WyndtEssex = GetHandle("Rodriguez");

	//Object_Rodnav1 = GetHandle("rodnav");
	//Object_Rodnav2 = GetHandle("rodnav2");
	//Object_Krudernav = GetHandle("Krudernav");
	Object_Corbernav = GetHandle("Corbernav");
	Object_Condor = GetHandle("condor");

	//Object_LandingZone = GetHandle("landing zone");
	//Object_ConvoyHalt = GetHandle("convoy halt");
	//Object_HardingNav = GetHandle("harding_nav");
	//Object_Red = GetHandle("red");
	//Object_Blue = GetHandle("blue");

	Object_ServiceBay = GetHandle("unnamed_ibsbay");
	Object_Carrier = GetHandle("unnamed_ivcarrs");
}

void EDF00Mission::AddObject(Handle h)
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

bool EDF00Mission::Load(bool missionSave)
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


bool EDF00Mission::PostLoad(bool missionSave)
{
	if (missionSave)
		return true;

	bool ret = true;

	ConvertHandles(h_array, h_count);

	//ret = ret && PUPMgr::PostLoad(missionSave);
	return ret;
}


bool EDF00Mission::Save(bool missionSave)
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

void EDF00Mission::Execute(void)
{
	if (!m_DidOneTimeInit)
		Init();

	if (!m_PlayerCanMove)
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
	}

	switch(m_convoyStateMachine)
	{
	case 0: // any special setup, could probably replace with 1 time init part
		m_convoyStateMachine++;
		break;
	case 1: // start the dropship effects
		StartEarthQuake(1.5f);
		Object_Player = GetPlayerHandle();
		Position_LandingZone3 = GetPosition(Object_Player);
		Object_Stayput = BuildObject("stayput",0,Position_LandingZone3);
		m_convoyWaitTillTime = m_ElapsedGameTime + (1 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 2:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 3:
		Ally(1,9);
		UnAlly(9,2);
		IFace_Exec("merc01.cfg");
		IFace_Activate("INFO1");
		IFace_Activate("Start1");
		m_convoyStateMachine++;
		FreeCamera();
		break;
	case 4:
		if(CameraReady())
		{
			Vector pos = GetPosition(Object_Player);
			Vector ang = GetFront(Object_Player);
			pos.y += 3.0f;
			SetCameraPosition(pos,ang);
			IFace_Deactivate("Start1");
			IFace_Activate("Message");
			m_convoyStateMachine++;
		}
		break;
	case 5:
		{
			int ifaceVal = IFace_GetInteger("images.page");
			if (ifaceVal == 4)
				m_convoyStateMachine++;
		}
		break;
	case 6:
		IFace_Deactivate("INFO1");
		IFace_Deactivate("Message");
		IFace_Exec("mercedf1.cfg");
		IFace_Activate("INFO");

		Object_CarrierLaunchCamDummy = BuildObject("dummy",2,Position_LandingZone1);
		//Position_HardingNav1 = GetPosition(Object_HardingNav);
		//Position_ScoutSpawning2 = GetPosition(Object_Blue);
		//Position_ScoutSpawning1 = GetPosition(Object_Red);
		//RemoveObject(Object_HardingNav);
		//RemoveObject(Object_Blue);
		//RemoveObject(Object_Red);
		Position_ServiceBay = GetPosition(Object_ServiceBay);
		//Position_LandingZone = GetPosition(Object_LandingZone);
		//RemoveObject(Object_LandingZone);
		SetGroup(Object_WyndtEssex,10);
		SetObjectiveName(Object_WyndtEssex,"Wyndt-Essex");
		//Object_Hardin = BuildObject(ODF_FRIEND_SCOUT,9,Position_HardingNav1);
		Object_Hardin = BuildObject(ODF_FRIEND_SCOUT,9,"hardin_spawn");
		//Object_Hardin = BuildObject(ODF_FRIEND_SCOUT,9,GetVectorFromPath("hardin_spawn"));
		SetObjectiveName(Object_Hardin,"Hardin");
		Object_Scout1 = BuildObject(ODF_FRIEND_SCOUT,9,"red_spawn");
		Object_Scout2 = BuildObject(ODF_FRIEND_SCOUT,9,"red_spawn");
		Object_Scout3 = BuildObject(ODF_FRIEND_SCOUT,9,"blue_spawn");
		Object_ServTruck1 = BuildObject(ODF_FRIEND_SERV,1,"blue_spawn");
		SetGroup(Object_ServTruck1,10);
		{
			Vector Position_LandingZone = GetVectorFromPath("landing_zone");
			Position_LandingZone.x += 1;
			Position_LandingZone.y += 1;
			Position_LandingZone.z += 1;
			Object_ServTruck2 = BuildObject(ODF_FRIEND_SERV,9,Position_LandingZone);
			Position_LandingZone.x += 10;
			//Position_LandingZone.y += 0;
			//Position_LandingZone.z += 0;
			Object_Cargo1 = BuildObject(ODF_FRIEND_CARGO,9,Position_LandingZone);
			Position_LandingZone.x += -15;
			//Position_LandingZone.y += 0;
			//Position_LandingZone.z += 0;
			Object_Cargo2 = BuildObject(ODF_FRIEND_CARGO,9,Position_LandingZone);
		}
		m_convoyStateMachine++;
		break;
	case 7:
		{
			int ifaceVal = IFace_GetInteger("images.page");
			if (ifaceVal == 3)
			{
				FreeFinish();
				m_convoyStateMachine++;
			}
		}
		break;
	case 8:
		m_convoyStateMachine++;
		break;
	case 9:
		if(CameraFinish())
			m_convoyStateMachine++;
		break;
	case 10:
		IFace_Deactivate("INFO");
		Goto(Object_Cargo1,"convoy",1);
		m_convoyWaitTillTime = m_ElapsedGameTime + (1 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 11:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 12:
		UpdateEarthQuake(4.0);
		LookAt(Object_WyndtEssex,Object_Player,1);
		m_convoyWaitTillTime = m_ElapsedGameTime + (1 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 13:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 14:
		AudioMessage("mercury_02.wav");
		m_convoyWaitTillTime = m_ElapsedGameTime + (9 * m_GameTPS); // maybe use "IsAudioMessageDone" instead?
		m_convoyStateMachine++;
		break;
	case 15:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 16:
		SetObjectiveOn(Object_Cargo2);
		AddObjective(OBJECTIVE_TEXT_1, WHITE);
		StopEarthQuake();
		SetAnimation(Object_Condor,"deploy",1);
		StartAnimation(Object_Condor);
		AudioMessage("dropdoor.wav");
		RemoveObject(Object_Stayput);
		m_PlayerCanMove = true;
		Goto(Object_Scout2,"convoy",1);
		Defend2(Object_WyndtEssex,Object_Cargo2,1);
		Follow(Object_ServTruck1,Object_Cargo2,1);
		Follow(Object_ServTruck2,Object_Cargo1,1);
		Follow(Object_Hardin,Object_Cargo1,1);
		Follow(Object_Scout1,Object_Cargo1,1);
		Goto(Object_Scout3,"convoy",1);
		m_EnableFailCheck = true;
		m_convoyWaitTillTime = m_ElapsedGameTime + (5 * m_GameTPS); // maybe use "IsAudioMessageDone" instead?
		m_convoyStateMachine++;
		break;
	case 17:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 18:
		Goto(Object_Cargo2,"convoy",1);
		m_convoyWaitTillTime = m_ElapsedGameTime + (30 * m_GameTPS); // maybe use "IsAudioMessageDone" instead?
		m_convoyStateMachine++;
		break;
	case 19:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 20:
		Object_Nadir1 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
		SetPerceivedTeam(Object_Nadir1,1);
		Attack(Object_Nadir1,Object_Cargo2,1);
		AudioMessage("mercury_02a.wav");
		m_convoyWaitTillTime = m_ElapsedGameTime + (13 * m_GameTPS); // maybe use "IsAudioMessageDone" instead?
		m_convoyStateMachine++;
		break;
	case 21:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 22:
		SetPerceivedTeam(Object_Nadir1,2);
		m_convoyStateMachine++;
		break;
	case 23:
		{
			Dist dist = GetDistance(Object_Cargo2,"convoy_halt");
			if (dist <= 50)
			{
				SetObjectiveOff(Object_Cargo2);
				AudioMessage("mercury_03.wav");
				m_convoyWaitTillTime = m_ElapsedGameTime + (19 * m_GameTPS); // maybe use "IsAudioMessageDone" instead?
				m_convoyStateMachine++;
			}
		}
		break;
	case 24:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 25:
		ClearObjectives();
		AddObjective(OBJECTIVE_TEXT_2,WHITE);
		m_runPowerAIStateMachine = true;
		m_runPowerPlayerStateMachine = true;
		m_convoyStateMachine++;
		break;
	case 26:
		{
			TeamNum radar1 = GetTeamNum(Object_Radar1);
			TeamNum radar2 = GetTeamNum(Object_Radar2);
			if(radar1 == 0 && radar2 == 0)
			{
				SetTeamNum(Object_Gun1,0);
				SetTeamNum(Object_Gun2,0);
				SetTeamNum(Object_Gun3,0);
				SetTeamNum(Object_Gun4,0);
				SetTeamNum(Object_Gun5,0);
				SetTeamNum(Object_Gun6,0);
				SetTeamNum(Object_Gun7,0);
				SetTeamNum(Object_Gun8,0);
				m_convoyWaitTillTime = m_ElapsedGameTime + (8 * m_GameTPS);
				m_convoyStateMachine++;
			}
		}
		break;
	case 27:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 28:
		AudioMessage("mercury_07.wav");
		AddObjective(OBJECTIVE_TEXT_11,WHITE);
		m_convoyStateMachine++;
		break;
	case 29:
		if(m_ConvoyContinueToBase)
		{
			Goto(Object_Hardin,"hardin",1);
			m_convoyWaitTillTime = m_ElapsedGameTime + (15 * m_GameTPS);
			m_convoyStateMachine++;
		}
		break;
	case 30:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 31:
		Goto(Object_WyndtEssex,"hardin",1);
		m_convoyWaitTillTime = m_ElapsedGameTime + (5 * m_GameTPS);
		m_convoyStateMachine++;
	case 32:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 33:
		Goto(Object_Cargo1,"transport1",1);
		m_convoyWaitTillTime = m_ElapsedGameTime + (10 * m_GameTPS);
		m_convoyStateMachine++;
	case 34:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 35:
		Goto(Object_Cargo2,"transport1",1);
		m_convoyStateMachine++;
	case 36:
		{
			Dist d1 = GetDistance(Object_Cargo1,Object_ServiceBay);
			Dist d2 = GetDistance(Object_Cargo2,Object_ServiceBay);
			Dist d3 = GetDistance(Object_WyndtEssex,Object_ServiceBay);
			if(d1 <= 300 && d2 <= 300 && d3 <= 300)
			{
				m_EnableFailCheck = false;
				m_runPowerPlayerStateMachine = false;
				SetObjectiveOn(Object_Hardin);
				Ally(2,9);
				SetTeamNum(Object_Scout2,9);
				SetTeamNum(Object_Scout1,9);
				SetTeamNum(Object_Scout3,9);
				Retreat(Object_Scout2,"hardin",1);
				Retreat(Object_Scout1,"hardin",1);
				Retreat(Object_Scout3,"hardin",1);
				ClearObjectives();
				LookAt(Object_Hardin,Object_Player,1);
				AddObjective(OBJECTIVE_TEXT_6,WHITE);
				AudioMessage("mercury_08.wav");
				SetObjectiveName(Object_Corbernav,"Enter StormPetrel here");
				SetTeamNum(Object_Gun1,0);
				SetTeamNum(Object_Gun2,1);
				SetTeamNum(Object_Gun3,0);
				SetTeamNum(Object_Gun4,1);
				SetTeamNum(Object_Gun5,0);
				SetTeamNum(Object_Gun6,1);
				SetTeamNum(Object_Gun7,0);
				SetTeamNum(Object_Gun8,0);
				m_convoyStateMachine++;
			}
		}
		break;
	case 37:
		Object_Player = GetPlayerHandle();
		{
			Dist dist = GetDistance(Object_Hardin,Object_Player);
			if(dist <= 200)
				m_convoyStateMachine++;
		}
		break;
	case 38:
		Goto(Object_WyndtEssex,Object_ServiceBay,1);
		Goto(Object_Scout2,Object_ServiceBay,1);
		Goto(Object_Scout3,Object_ServiceBay,1);
		Goto(Object_Scout1,Object_ServiceBay,1);
		HopOut(Object_Hardin);
		SetTeamNum(Object_Corbernav,1);
		SetObjectiveOn(Object_Corbernav);
		SetObjectiveOff(Object_Hardin);
		m_convoyStateMachine++;
		break;
	case 39:
		Object_Player = GetPlayerHandle();
		{
			Dist dist = GetDistance(Object_Corbernav,Object_Player);
			if(dist < 10)
				m_convoyStateMachine++;
		}
		break;
	case 40:
		StartEarthQuake(4.0);
		m_CerbRoutine = false;
		ClearObjectives();
		AddObjective(OBJECTIVE_TEXT_5,GREEN);
		AudioMessage("mercury_08a.wav");
		HopOut(Object_Scout2);
		HopOut(Object_Scout3);
		HopOut(Object_Scout1);
		m_convoyWaitTillTime = m_ElapsedGameTime + (3 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 41:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 42:
		//Object_HardingNav = BuildObject("ibnav",2,Position_HardingNav2);
		RemoveObject(Object_Cargo1);
		RemoveObject(Object_Cargo2);
		m_Routine4Enable = true;
		{
			Matrix mat = Matrix();
			GetPosition(Object_Carrier, mat);
			RemoveObject(Object_Carrier);
			Object_Carrier = BuildObject("ivcarrs_ani",1,mat);
		}
		m_convoyWaitTillTime = m_ElapsedGameTime + (7 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 43:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 44:
		AudioMessage("dropleav.wav");
		m_convoyWaitTillTime = m_ElapsedGameTime + (10 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 45:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 46:
		AudioMessage("mercury_09.wav");
		m_convoyWaitTillTime = m_ElapsedGameTime + (12 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 47:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 48:
		{
			Matrix mat = Matrix();
			GetPosition(Object_Carrier, mat);
			RemoveObject(Object_Carrier);
			Object_Carrier = BuildObject("ivcarrs",1,mat);
		}
		m_convoyWaitTillTime = m_ElapsedGameTime + (12 * m_GameTPS);
		m_convoyStateMachine++;
		break;
	case 49:
		if (m_ElapsedGameTime >= m_convoyWaitTillTime)
			m_convoyStateMachine++;
		break;
	case 50:
		StopEarthQuake();
		SucceedMission(0,"winmerc.des");
		break;
	}

	if(m_runPowerAIStateMachine)
	{
		switch(m_powerAIStateMachine)
		{
		case 0: // any special setup, could probably replace with 1 time init part
			m_powerAIStateMachine++;
			break;
		case 1:
			Goto(Object_Scout2,"scout1",1);
			Goto(Object_Scout1,"scout1",1);
			m_powerAIStateMachine++;
			break;
		case 2:
			{
				//Dist dist = GetDistance(Object_Scout1,Object_Krudernav);
				Dist dist = GetDistance(Object_Scout1,"red_goto_power");
				if(dist <= 20)
					m_powerAIStateMachine++;
			}
			break;
		case 3:
			RemoveObject(Object_Gun9);
			Attack(Object_Scout2,Object_Power7,1);
			Attack(Object_Scout1,Object_Power7,1);
			m_powerAIStateMachine++;
		case 4:
			if(!IsAround(Object_Power7))
				m_powerAIStateMachine++;
			break;
		case 5:
			SetCurAmmo(Object_Scout2,800);
			SetCurAmmo(Object_Scout1,800);
			Attack(Object_Scout2,Object_Power6,1);
			Attack(Object_Scout1,Object_Power6,1);
			m_powerAIStateMachine++;
		case 6:
			if(!IsAround(Object_Power6))
				m_powerAIStateMachine++;
			break;
		case 7:
			SetCurAmmo(Object_Scout2,800);
			SetCurAmmo(Object_Scout1,800);
			Attack(Object_Scout2,Object_Power5,1);
			Attack(Object_Scout1,Object_Power5,1);
			m_powerAIStateMachine++;
			break;
		case 8:
			if(!IsAround(Object_Power5))
				m_powerAIStateMachine++;
			break;
		case 9:
			SetTeamNum(Object_Radar1,0);
			Goto(Object_Scout2,"scout3",0);
			Goto(Object_Scout1,"scout3",0);
			m_powerAIStateMachine++;
			break;
		}
	}

	if(m_runPowerPlayerStateMachine)
	{
		switch(m_powerPlayerStateMachine)
		{
		case 0: // setup
			m_powerPlayerStateMachine++;
			break;
		case 1:
			Object_Player = GetPlayerHandle();
			SetCurHealth(Object_Gun10,3000);
			SetObjectiveOn(Object_WyndtEssex);
			LookAt(Object_WyndtEssex,Object_Player,1);
			AudioMessage("mercury_04.wav"); // Essex >> Follow me leutenent
			Object_Nadir1 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Attack(Object_Nadir1,Object_WyndtEssex,1);
			//Position_Rodnav2 = GetPosition(Object_Rodnav2);
			//Object_NavForCerbUnit = BuildObject("ibnav",4,Position_NavForCerbUnit);
			Goto(Object_WyndtEssex,"rod1",1);
			SetMaxHealth(Object_Power1,800);
			SetMaxHealth(Object_Power2,800);
			SetMaxHealth(Object_Power3,800);
			SetMaxHealth(Object_Power4,800);
			m_powerPlayerStateMachine++;
			break;
		case 2:
			{
				//Dist dist = GetDistance(Object_WyndtEssex,Object_Rodnav1);
				Dist dist = GetDistance(Object_WyndtEssex,"blue_goto_power_1");
				if(dist <= 20)
					m_powerPlayerStateMachine++;
			}
			break;
		case 3:
			{
				Dist dist = GetDistance(Object_Player,Object_WyndtEssex);
				if(dist <= 50)
					m_powerPlayerStateMachine++;
			}
			break;
		case 4:
			LookAt(Object_WyndtEssex,Object_Player,1);
			AudioMessage("mercury_05.wav");
			m_powerPlayerWaitTillTime = m_ElapsedGameTime + (1 * m_GameTPS);
			m_powerPlayerStateMachine++;
			break;
		case 5:
			if(m_ElapsedGameTime >= m_powerPlayerWaitTillTime)
				m_powerPlayerStateMachine++;
			break;
		case 6:
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_3,WHITE);
			m_CerbRoutine = true;
			SetObjectiveOn(Object_Gun10);
			m_powerPlayerStateMachine++;
			break;
		case 7:
			if(!IsAround(Object_Gun10))
				m_powerPlayerStateMachine++;
			break;
		case 8:
			//Goto(Object_WyndtEssex,Object_Rodnav2,1);
			Goto(Object_WyndtEssex,"blue_goto_power_2",1);
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_3,GREEN);
			AddObjective(OBJECTIVE_TEXT_4,WHITE);
			m_powerPlayerWaitTillTime = m_ElapsedGameTime + (22 * m_GameTPS);
			m_powerPlayerStateMachine++;
			break;
		case 9:
			if(m_ElapsedGameTime >= m_powerPlayerWaitTillTime)
				m_powerPlayerStateMachine++;
			break;
		case 10:
			//SetVectorPosition(Object_WyndtEssex,Position_Rodnav2);
			SetPosition(Object_WyndtEssex,"blue_goto_power_2");
			m_powerPlayerStateMachine++;
			break;
		case 11:
			if(!IsAround(Object_Power1)
			&& !IsAround(Object_Power2)
			&& !IsAround(Object_Power3)
			&& !IsAround(Object_Power4))
				m_powerPlayerStateMachine++;
			break;
		case 12:
			SetTeamNum(Object_Radar2,0);
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_2,WHITE);
			AudioMessage("mercury_06.wav");
			Goto(Object_WyndtEssex,"path_1",1);
			Object_Nadir1 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Nadir2 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Player = GetPlayerHandle();
			Attack(Object_Nadir1,Object_WyndtEssex,1);
			Attack(Object_Nadir2,Object_Player,1);
			m_powerPlayerStateMachine++;
			break;
		case 13:
			{
				//Dist dist = GetDistance(Object_ConvoyHalt,Object_WyndtEssex);
				Dist dist = GetDistance(Object_WyndtEssex,"convoy_halt");
				if(dist <= 50)
					m_powerPlayerStateMachine++;
			}
			break;
		case 14:
			SetObjectiveOff(Object_WyndtEssex);
			SetTeamNum(Object_Scout2,1);
			SetTeamNum(Object_Scout3,1);
			SetGroup(Object_Scout3,0);
			Follow(Object_Scout3,Object_Player,0);
			SetTeamNum(Object_Scout1,1);
			SetTeamNum(Object_ServTruck2,1);
			SetGroup(Object_ServTruck1,8);
			SetGroup(Object_ServTruck2,8);
			Service(Object_ServTruck1,Object_Cargo1,0);
			Service(Object_ServTruck2,Object_Cargo2,0);
			m_powerPlayerWaitTillTime = m_ElapsedGameTime + (80 * m_GameTPS);
			m_powerPlayerStateMachine++;
			break;
		case 15:
			if(m_ElapsedGameTime >= m_powerPlayerWaitTillTime)
				m_powerPlayerStateMachine++;
			break;
		case 16:
			Object_Player = GetPlayerHandle();
			Object_Nadir1 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Nadir2 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Nadir3 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Attack(Object_Nadir1,Object_WyndtEssex,1);
			Attack(Object_Nadir2,Object_Cargo2,1);
			Attack(Object_Nadir3,Object_Player,1);
			m_powerPlayerStateMachine++;
			break;
		case 17:
			if(!IsAround(Object_Nadir1)
			&& !IsAround(Object_Nadir2)
			&& !IsAround(Object_Nadir3))
				m_powerPlayerStateMachine++;
			break;
		case 18:
			m_ConvoyContinueToBase = true;
			m_powerPlayerStateMachine++;
		case 19: // repeated attack wave delay set
			m_powerPlayerWaitTillTime = m_ElapsedGameTime + (35 * m_GameTPS);
			m_powerPlayerStateMachine++;
		case 20: // repeated attack wave delay
			if(m_ElapsedGameTime >= m_powerPlayerWaitTillTime)
				m_powerPlayerStateMachine++;
			break;
		case 21: // repeated attack wave spawn
			Object_Nadir1 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Nadir2 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Nadir3 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Object_Nadir4 = BuildObject(ODF_ENEMY_DRONE,2,"NadirAttackSpawn");
			Attack(Object_Nadir1,Object_Player,1);
			Attack(Object_Nadir2,Object_Player,1);
			Goto(Object_Nadir3,"hardin",1);
			Goto(Object_Nadir4,"hardin",1);
			m_powerPlayerStateMachine = 19; // start attack wave cycle again
		}
	}

	if(m_EnableFailCheck)
	{
		if(!IsAround(Object_WyndtEssex))
		{
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_8,RED);
			FailMission(10,"rodmerc.des");
		}else if(!IsAround(Object_Cargo2))
		{
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_10,RED);
			FailMission(10,"transmerc.des");
		}else if(!IsAround(Object_Hardin))
		{
			ClearObjectives();
			AddObjective(OBJECTIVE_TEXT_9,RED);
			FailMission(10,"hardmerc.des");
		}
	}

	if(m_Routine4Enable)
	{
		switch(m_Routine4StateMachine)
		{
		case 0:
			//SetVectorPosition(Object_Player,Position_LandingZone0);
			m_runPowerPlayerStateMachine = false;
			m_Routine4StateMachine++;
		case 1:
			if(CameraReady())
				m_Routine4StateMachine++;
			break;			
		case 2:
			if(CameraPath("end",4000,1000,Object_Carrier)) // PanDone instead?
				m_Routine4StateMachine++;
			break;
		case 3:
			//CameraObject(Object_HardingNav,Position_CarrierLaunchCamDummy.x,Position_CarrierLaunchCamDummy.y,Position_CarrierLaunchCamDummy.z,Object_CarrierLaunchCamDummy);
			m_Routine4WaitTillTime = m_ElapsedGameTime + (27 * m_GameTPS);
			m_Routine4StateMachine++;
		case 4:
			if(m_ElapsedGameTime >= m_Routine4WaitTillTime)
				m_Routine4StateMachine++;
			break;
		case 5:
			CameraFinish();
			m_Routine4StateMachine++;
			break;
		}
	}

	if(m_CerbRoutine)
	{
		switch(m_CerbStateMachine)
		{
		case 0:
			m_CerbStateMachine++;
		case 1:
			//Object_CerbUnit = BuildObject("cvscout10",4,Position_Rodnav2);
			Object_CerbUnit = BuildObject("cvscout10",4,"blue_goto_power_2");
			SetObjectiveName(Object_CerbUnit,"Unknown");
			Goto(Object_CerbUnit,Object_Gun10,1);
			m_CerbWaitTillTime = m_ElapsedGameTime + (8 * m_GameTPS);
			m_CerbStateMachine++;
		case 2:
			if(m_ElapsedGameTime >= m_CerbWaitTillTime)
				m_CerbStateMachine++;
			break;
		case 3:
			Retreat(Object_CerbUnit,"path_2",1);
			m_CerbStateMachine++;
		case 4:
			{
				//Dist dist = GetDistance(Object_CerbUnit,Object_NavForCerbUnit);
				Dist dist = GetDistance(Object_CerbUnit,"path_2",3);
				if(dist <= 100)
				{
					m_CerbStateMachine = 7;
				}else{
					Handle Object_CerbUnitShotBy = GetWhoShotMe(Object_CerbUnit);
					if(IsPlayer(Object_CerbUnitShotBy))
					{
						AudioMessage("mercury_07a.wav");
						m_CerbStateMachine++;
					}
				}
			}
			break;
		case 5:
			{
				Dist dist = GetDistance(Object_CerbUnit,"path_2",3);
				if(dist <= 10)
				{
					m_CerbStateMachine = 7;
				}else{
					if(GetCurHealth(Object_CerbUnit) <= 1000)
					{
						Object_Player = GetPlayerHandle();
						EjectPilot(Object_Player);
						AddObjective(OBJECTIVE_TEXT_12,VIOLET);
						m_CerbStateMachine++;
					}
				}
			}
			break;
		case 6: // dead state, nothing here
			break;
		case 7:
			RemoveObject(Object_CerbUnit);
			m_CerbStateMachine++;
			break;
		}
	}

	if(m_PreventPowerDamage)
	{
		if(!m_SetGun10Heath)
		{
			SetCurHealth(Object_Gun10,20000);

			SetCurHealth(Object_Gun10,3000);
			SetCurHealth(Object_Power1,800);
			SetCurHealth(Object_Power2,800);
			SetCurHealth(Object_Power3,800);
			SetCurHealth(Object_Power4,800);

			m_SetGun10Heath = true;
		}

		Handle Object_Gun10ShotBy = GetWhoShotMe(Object_Gun10);
		if(IsPlayer(Object_Gun10ShotBy))
		{
			SetCurHealth(Object_Gun10,3000);
			SetCurHealth(Object_Power1,800);
			SetCurHealth(Object_Power2,800);
			SetCurHealth(Object_Power3,800);
			SetCurHealth(Object_Power4,800);
		}
	}

	if((m_convoyStateMachineLast != m_convoyStateMachine)
		|| (m_powerAIStateMachineLast != m_powerAIStateMachine)
		|| (m_powerPlayerStateMachineLast != m_powerPlayerStateMachine)
		|| (m_convoyStateMachineLast != m_convoyStateMachine)
		|| (m_Routine4StateMachineLast != m_Routine4StateMachine)
		|| (m_CerbStateMachineLast != m_CerbStateMachine))
	{
		sprintf_s(TempMsgString, "Convoy: %d, AiPower: %d, PlayerPower: %d, Rout4: %d, Cerb: %d", m_convoyStateMachine, m_powerAIStateMachine, m_powerPlayerStateMachine, m_Routine4StateMachine, m_CerbStateMachine);
		PrintConsoleMessage(TempMsgString);
		m_convoyStateMachineLast = m_convoyStateMachine;
		m_powerAIStateMachineLast = m_powerAIStateMachine;
		m_powerPlayerStateMachineLast = m_powerPlayerStateMachine;
		m_Routine4StateMachineLast = m_Routine4StateMachine;
		m_CerbStateMachineLast = m_CerbStateMachine;
	}

	m_ElapsedGameTime++;
}

DLLBase * BuildMission(void)
{
	return new EDF00Mission();
}
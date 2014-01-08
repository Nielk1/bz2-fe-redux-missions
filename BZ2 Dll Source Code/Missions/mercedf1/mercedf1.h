#ifndef MERCEDF1_H__
#define MERCEDF1_H__

#include "..\Shared\DLLBase.h"

class EDF00Mission : public DLLBase//public SPMission 
{
public:
	EDF00Mission(void)
	{
		m_GameTPS = 10; // default

		AllowRandomTracks(false); // Allow random music if the user wants it
		b_count = &b_last - &b_first - 1;
		b_array = &b_first + 1;

		f_count = &f_last - &f_first - 1;
		f_array = &f_first + 1;

		h_count = &h_last - &h_first - 1;
		h_array = &h_first + 1;

		i_count = &i_last - &i_first - 1;
		i_array = &i_first + 1;

		v_count = &v_last - &v_first - 1;
		v_array = &v_first + 1;
	}

	void Init(void);
	void AddObject(Handle h);
	bool Load(bool missionSave);
	bool PostLoad(bool missionSave);
	bool Save(bool missionSave);
	void Execute(void);

	// bools
	bool
		b_first,
		m_DidOneTimeInit,
		m_ConvoyContinueToBase,
		m_EnableFailCheck,
		m_runPowerAIStateMachine,
		m_runPowerPlayerStateMachine,
		m_Routine4Enable,
		m_CerbRoutine,
		m_PreventPowerDamage,
		m_SetGun10Heath,
		m_PlayerCanMove,
		b_last;

	// floats
	float
		f_first,
		f_last;

	// handles
	Handle
		h_first,
		Object_Player,
		Object_CarrierLaunchCamDummy,
		//Object_LandingZone,
		//Object_ConvoyHalt,
		//Object_HardingNav,
		//Object_Red,
		//Object_Blue,
		Object_Radar1,
		Object_Radar2,
		Object_Gun1,
		Object_Gun2,
		Object_Gun3,
		Object_Gun4,
		Object_Gun5,
		Object_Gun6,
		Object_Gun7,
		Object_Gun8,
		Object_Gun9,
		Object_Gun10,
		Object_ServiceBay,
		Object_Carrier,
		Object_WyndtEssex,
		Object_Hardin,
		Object_Scout1,
		Object_Scout2,
		Object_Scout3,
		Object_ServTruck1,
		Object_ServTruck2,
		Object_Cargo1,
		Object_Cargo2,
		Object_Condor,
		Object_Corbernav,
		Object_Stayput,
		Object_Power1,
		Object_Power2,
		Object_Power3,
		Object_Power4,
		Object_Power5,
		Object_Power6,
		Object_Power7,
		//Object_Krudernav,
		//Object_Rodnav1,
		//Object_Rodnav2,
		//Object_NavForCerbUnit,
		Object_Nadir1,
		Object_Nadir2,
		Object_Nadir3,
		Object_Nadir4,
		Object_CerbUnit,
		h_last;

	// integers
	int
		i_first,
		m_ElapsedGameTime,
		m_convoyStateMachine,
		m_convoyWaitTillTime,
		m_powerAIStateMachine,
		m_powerPlayerStateMachine,
		m_powerPlayerWaitTillTime,
		m_Routine4StateMachine,
		m_Routine4WaitTillTime,
		m_CerbStateMachine,
		m_CerbWaitTillTime,
		i_last;

	Vector
		v_first,
		Position_LandingZone3,
		Position_LandingZone1,
		//Position_LandingZone0,
		//Position_HardingNav1,
		//Position_HardingNav2,
		//Position_ScoutSpawning2,
		//Position_ScoutSpawning1,
		Position_ServiceBay,
		//Position_LandingZone,
		//Position_LandingZone2,
		//Position_Rodnav2,
		Position_NavForCerbUnit,
		Position_CarrierLaunchCamDummy,
		v_last;

protected:
	bool *b_array;
	int b_count;

	float *f_array;
	int f_count;

	int *h_array;
	int h_count;

	int *i_array;
	int i_count;

	Vector *v_array;
	int v_count;

private:
	int m_GameTPS;
	char TempMsgString[1024];

	int m_convoyStateMachineLast;
	int m_powerAIStateMachineLast;
	int m_powerPlayerStateMachineLast;
	int m_Routine4StateMachineLast;
	int m_CerbStateMachineLast;

	Vector GetVectorFromPath(Name path, int point = 0);
	int UnsignedToSigned(unsigned int x);
};

#endif // MERCEDF1_H__
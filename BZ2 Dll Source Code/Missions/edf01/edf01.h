#ifndef edf01_H__
#define edf01_H__

#include "..\Shared\DLLBase.h"

class EDF01Mission : public DLLBase//public SPMission 
{
public:
	EDF01Mission(void)
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
		//m_ConvoyContinueToBase,
		//m_EnableFailCheck,
		//m_runPowerAIStateMachine,
		//m_runPowerPlayerStateMachine,
		//m_Routine4Enable,
		//m_CerbRoutine,
		//m_PreventPowerDamage,
		//m_SetGun10Heath,
		//m_PlayerCanMove,
		m_SpawnAttackerLoop1,
		m_SpawnAttackerLoop2,
		b_last;

	// floats
	float
		f_first,
		f_last;

	// handles
	Handle
		h_first,
		Object_Player,
		Object_Observer1,
		Object_Observer2,
		Object_CurrentPowerSource,
		Object_Buddy1,
		Object_Buddy2,
		Object_Buddy3,
		Object_Buddy4,
		Object_Buddy5,
		Object_Buddy6,
		Object_TempCreation,
		Object_TmpForChecks,
		Object_NewObjectCheck,
		Object_RemoveObjectCheck,
		Object_DropShip,
		Object_TempSurvivor,
		Object_SurvivorForDropoffCheck,
		Object_SurvivorForPilotCheck,
		Object_WaitingSurvivor,
		Object_SurvivorArray[10],
		Object_NearSurvivor,
		Object31,
		Object_SurvivorDropoff,
		Object_FriendTurret3,
		Object_FriendTurret2,
		Object_APC1,
		Object_APC2,
		Object_TempForAttackers,
		h_last;

	// integers
	int
		i_first,
		m_ElapsedGameTime,
		m_mainStateMachine,
		m_mainWaitTillTime,
		Variable_ScannedPercent,
		Variable_PowerSourceNumber,
		Variable_SurvivorNumber,
		Variable_SurvivorNumber2,
		Variable_SurvivorNumber3,
		Variable_SurvivorNumber4,
		Variable_SurvivorNumber5,
		Variable_SurvivorNumber6,
		Variable_SurvivorSafeCount,
		Variable10,
		Variable11,
		Variable12,
		m_SpawnAttackerLoopStateMachine,
		m_SpawnAttackerLoopWaitTillTime,
		m_SpawnAttackerLoopStateMachine2,
		m_SpawnAttackerLoopWaitTillTime2,
		m_Routine2StateMachine,
		m_Routine2WaitTillTime,
		i_last;

	Vector
		v_first,
		Position1,
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

	Vector GetVectorFromPath(Name path, int point = 0);
	int UnsignedToSigned(unsigned int x);

	int m_mainStateMachineLast;
	int m_SpawnAttackerLoopStateMachineLast;
	int m_SpawnAttackerLoopStateMachine2Last;
	int m_Routine2StateMachineLast;
};

#endif // edf01_H__
#ifndef edf01_H__
#define edf01_H__

//#include "..\Shared\DLLBase.h"
#include "..\Shared\RebuildMission.h"

class EDF01Mission : public RebuildMission 
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
	void DeleteObject(Handle h);
	bool Load(bool missionSave);
	bool PostLoad(bool missionSave);
	bool Save(bool missionSave);
	void Execute(void);

	// bools
	bool
		b_first,

		m_DidOneTimeInit,

		m_StopPlayerMoving,

		m_ForceNavsToStay,

		m_ShutlzGoingToDropship,
		m_ShutlzCamActive,

		m_SpawnAttackerLoop,
		m_FirstResponsiveAttackWave,

		m_SurvivorChecksActive,
		m_NotedLostSurvivor,
		m_SurvivorNav1ShowCount,
		m_SurvivorNav2ShowCount,

		m_StepOneOfTankCountFailDone,
		m_StepOneOfEnemySpawnCountFailDone,

		b_last;

	// floats
	float
		f_first,
		f_last;

	// handles
	Handle
		h_first,

		m_BuddyTanks[6],
		m_FriendTurret3,
		m_FriendTurret2,
		m_APC1,
		m_APC2,

		m_ShultzTank,
		m_ShultzPilot,

		m_HadeanObserver1,
		m_HadeanObserver2,

		m_TargetPowerSourceForScanning,

		m_Survivors[10],
		m_SurvivorDropoffDropship,

		m_SurvivorDropoffNav,
		m_SurvivorNav1,
		m_SurvivorNav2,

		h_last;

	// integers
	int
		i_first,

		m_ElapsedGameTime,

		m_ShutlzPilotActionWaitTillTime,

		m_ScanningMissionPhase,
		m_ScanningMissionPhaseWaitTillTime,
		m_ScannedPercent,
		m_PowerSourceNumber,

		m_SurvivorMissionPhase,
		m_SurvivorMissionPhaseWaitTillTime,
		m_SurvivorChecksWaitTillTime,
		m_SurvivorSafeCount,
		m_SurvivorNumberForBuddyLabeling,

		m_SpawnAttackerLoopStateMachine,
		m_SpawnAttackerLoopWaitTillTime,
		m_SpawnAttackerLoopStateMachine2,
		m_SpawnAttackerLoopWaitTillTime2,
		
		m_AttackCountFailCheckWaitTillTime,
		m_AttackerCountForFailCheck,

		i_last;

	Vector
		v_first,

		m_ShultzEjectCameraBase,

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

	void Preload();
	void MakeNavs();

	//Vector GetVectorFromPath(Name path, int point = 0);
	//int UnsignedToSigned(unsigned int x);
	//Handle ReplaceObject(Handle h, char* odf, bool keepHealth = false);

	//char m_StringWorkBuffer[200];
};

#endif // edf01_H__
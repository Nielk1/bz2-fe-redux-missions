#ifndef edf02_H__
#define edf02_H__

//#include "..\Shared\DLLBase.h"
#include "..\Shared\RebuildMission.h"
//#include "..\Shared\ScriptEngine.h"

class EDF02Mission : public RebuildMission 
{
public:
	EDF02Mission() : RebuildMission()
	{
		//m_GameTPS = 10; // default
		RebuildMission::RebuildMission();

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

	void OnPortalProximity(Handle Portal, Handle NearVehicle);

	// bools
	bool
		b_first,

		m_DidOneTimeInit,
		m_Routine2Active,
		m_Routine3Active,
		m_Routine4Active,
		//m_Routine5Active,
		m_Routine6Active,

		Variable_Bool_StartDeployBase,

		b_last;

	// floats
	float
		f_first,

		Variable_GameTime_DinoSpawns,
		Variable_GameTime_MakeGuntowersFastEnough,
		Variable_GameTime_UseThatPortal,

		f_last;

	// handles
	Handle
		h_first,

		Object_FlyingDropship,
		Object_Player,
		Object_MissionNav,
		Object_TmpForSpawning,
		Object_Recycler,
		Object_Portal1,
		Object_Portal2,
		Object_Portal3,
		Object_Portal4,
		Object_Portal5,
		Object_Pool1,
		Object_DinoA,
		Object_DinoB,
		Object_Wave_Attackers[8],
		Object_StayPut,
		Object_DeployingDropship,
		Object_PossibleObjectNearPortal,
		Object_ObservedPortalUser1,
		Object_ObservedPortalUser2,

		h_last;

	// integers
	int
		i_first,

		m_Routine1,
		m_Routine1WaitTillTime,

		m_Routine2,
		m_Routine2WaitTillTime,

		m_Routine3,
		m_Routine3WaitTillTime,

		m_Routine4,
		m_Routine4WaitTillTime,

		m_Routine5,
		m_Routine5WaitTillTime,

		m_Routine6,
		m_Routine6WaitTillTime,

		m_Routine8,
		m_Routine8WaitTillTime,

		Variable_DefeatedAttackWaveCount,

		Variable_CountGuntowers,

		i_last;

	Vector
		v_first,

		Position_FlyingDropshipMoveTo,
		Position_FlyingDropshipStartAt,
		Position_PlayerPosition,

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
	char TempMsgString[1024];

	void Preload();

	//Vector GetVectorFromPath(Name path, int point = 0);
	//int UnsignedToSigned(unsigned int x);
	//Handle ReplaceObject(Handle h, char* odf, bool keepHealth = false);

	//char m_StringWorkBuffer[200];
};

#endif // edf01_H__
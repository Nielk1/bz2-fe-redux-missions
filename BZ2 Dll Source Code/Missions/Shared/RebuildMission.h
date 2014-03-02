#ifndef _RebuildMission_20140119_
#define _RebuildMission_20140119_

#define TURQUOISE RGB(0,255,255)
#define YELLOW RGB(255,255,0)
#define BLUE RGB(0,0,255)
#define VIOLET RGB(255,0,255)

#define CENTIGREES_TO_RADIANS 0.0001745329f

#include "..\..\source\fun3d\ScriptUtils.h"

#include "DLLBase.h"
#include "SPMission.h"

#include <crtdbg.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

class RebuildMission : public DLLBase {
public:
	RebuildMission(void);

	//virtual void Setup(void) = 0;
	virtual void Execute(void)
	{
		if(!m_DidOneTimeInit)
			EnableHighTPS(m_GameTPS);
	}
	//virtual void AddObject(Handle h)
	//{
	//}
	//virtual void DeleteObject(Handle h)
	//{
	//}
	//virtual void ProcessCommand(unsigned long crc)
	//{
	//}
	virtual EjectKillRetCodes PlayerEjected(void);
	virtual EjectKillRetCodes ObjectKilled(int DeadObjectHandle, int KillersHandle);
	virtual bool Save(bool missionSave);
	virtual bool Load(bool missionSave);
	//bool PostLoad(bool missionSave);

	//bool *b_array;
	//int b_count;

	//float *f_array;
	//int f_count;

	//int *h_array;
	//int h_count;

	//int *i_array;
	//int i_count;

protected:
	void Wait(int &timer, int seconds);
	bool Move(Handle object, int degPerSec, int cmPerSec, Vector destination);

	int m_ElapsedGameTime;
	int m_GameTPS;

private:
	int m_DidOneTimeInit;
};

Vector HRotateFront (Vector Front, float HAngleDifference);
Vector GetVectorFromPath(Name path, int point = 0);
Handle ReplaceObject(Handle h, char* odf, bool keepHealth = false);

#endif // _RebuildMission_20140119_
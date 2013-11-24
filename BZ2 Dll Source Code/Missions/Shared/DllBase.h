#ifndef _DLLBase_H__
#define _DLLBase_H__

#include "..\..\source\fun3d\ScriptUtils.h"

#include <crtdbg.h>
#include <stdlib.h>
#include <memory.h>

#pragma warning (disable : 4514) //unreferenced inline/local function has been removed
#pragma warning (disable : 4100) //unreferenced formal parameter
#pragma warning (disable : 4127)	// conditional expression is constant -- NEEDED FOR _ASSERTE


//
// Warnings we don't want
//
#pragma warning(disable: 4201 4505 4121)
#pragma warning(disable: 4324) // c:\src\bz2\trunk\source\fun3d\factory.h(167) : warning C4324: 'Factory' : structure was padded due to __declspec(align())

// Warnings we DO want
#pragma warning (error : 4013) //'function' undefined; assuming extern returning int
#pragma warning (error : 4020) // too many actual parameters (i.e. different from declaration)
#pragma warning (error : 4024) // formal parameter N different from declaration
#pragma warning (error : 4028) // formal parameter N different from declaration (not quite C4024)
#pragma warning (error : 4033) // function must return a value
#pragma warning (error : 4087) // function : declared with 'void' parameter list
#pragma warning (error : 4133) //'type' : incompatible types - from 'type1' to 'type2'
#pragma warning (error : 4307) // integral constant overflow
#pragma warning (error : 4700) // local variable 'foo' used without having been initialized
#pragma warning (error : 4701) // local variable 'foo' may used without having been initialized
#pragma warning (error : 4715) // function: not all control paths return a value
#pragma warning (error : 4265) // warning C4265: class has virtual functions, but destructor is not virtual
#pragma warning (error : 4905) // wide string literal cast to 'LPSTR'
#pragma warning (error : 4906) // string literal cast to 'LPWSTR'
#pragma warning (error : 4431) // 'missing type specifier - int assumed. Note: C no longer supports default-int
#pragma warning (error : 4806) // unsafe operation: no value of type 'bool' promoted to type 'int' can equal the given constant



#define DPID_UNKNOWN		0xFFFFFFFF

class DLLBase {
public:
	virtual ~DLLBase()
	{
	}

	// Set of virtual functions mirroring misnExport structure.
	virtual void InitialSetup(void);
	virtual bool Save(bool missionSave);
	virtual bool Load(bool missionSave);
	virtual bool PostLoad(bool missionSave);
	virtual void AddObject(Handle h);
	virtual void DeleteObject(Handle h);
	virtual void Execute(void);
	virtual void PostRun(void);
	virtual bool AddPlayer(DPID id, int Team, bool ShouldCreateThem);
	virtual void DeletePlayer(DPID id);
	virtual EjectKillRetCodes PlayerEjected(Handle DeadObjectHandle);
	virtual EjectKillRetCodes ObjectKilled(int DeadObjectHandle,int KillersHandle);
	virtual EjectKillRetCodes ObjectSniped(int DeadObjectHandle,int KillersHandle);
	virtual char *GetNextRandomVehicleODF(int Team);
	virtual void SetWorld(int nextWorld);
	virtual void ProcessCommand(unsigned long crc);
	virtual void SetRandomSeed(unsigned long seed);

};

// Function to create a new instance of your mission class. Must be present in project
DLLBase *BuildMission(void);

#endif // _DLLBase_H__

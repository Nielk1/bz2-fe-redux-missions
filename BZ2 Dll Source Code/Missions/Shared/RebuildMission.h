#ifndef _RebuildMission_20140119_
#define _RebuildMission_20140119_

#define TURQUOISE RGB(0,255,255)
#define YELLOW RGB(255,255,0)
#define BLUE RGB(0,0,255)
#define VIOLET RGB(255,0,255)

#define CENTIGREES_TO_RADIANS 0.0001745329f

#define MaxPortals 20
#define MaxLabelledPortals 12

#include "..\..\source\fun3d\ScriptUtils.h"

#include "DLLBase.h"
#include "SPMission.h"

#include <crtdbg.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <math.h>

class RebuildMission : public DLLBase {
public:
	RebuildMission(void);

	//virtual void Setup(void) = 0;
	virtual void Execute(void);
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
	virtual bool PostLoad(bool missionSave);


	virtual void OnPortalProximity(Handle Portal, Handle NearVehicle);

private:
	// bools
	bool
		b_first,

		m_DidOneTimeInit,
		CameraWaiting,
		CameraCanCut,

		b_last;

	// floats
	float
		f_first,
		f_last;

	// handles
	Handle
		h_first,
	
		LabelledPortals[MaxLabelledPortals],
		Portals[MaxPortals],
		PortalsDest[MaxPortals],

		h_last;

	// integers
	int
		i_first,

		PortalCount,
		SaveRoutineSpeed,
		LabelledPortalCount,
		PortalGroup[MaxLabelledPortals],

		i_last;

	bool *b_array;
	int b_count;

	float *f_array;
	int f_count;

	int *h_array;
	int h_count;

	int *i_array;
	int i_count;

	void Init(void);

protected:
	void Wait(int &timer, int seconds);
	bool Move(Handle object, int degPerSec, int cmPerSec, Vector destination);
	void SetPortal(Handle object);
	void SetPortalDest(Handle input, Handle output);
	void PortalFunctions();
	Handle GetPortalDest(Handle Portal);
	void Teleport(Handle object, Handle destination, int overshoot);

	int m_ElapsedGameTime;

	int m_GameTPS;
};

// This function rotates the directional vector clockwise in the horizontal plane by
// the specified angle in radians.  Function by B.S.
// _________________________________________________________________
Vector HRotateFront (Vector Front, float HAngleDifference)
{
	Vector FinalVector;
	double Angle;
	double Scale = sqrt(1 - Front.y * Front.y);

	// Get the horizontal angle of the initial directional vector.
	Angle = atan2(Front.x, Front.z);

	// Add the rotation amount to the current angle, and convert back into
	// a directional vector.
	Angle += HAngleDifference;
	FinalVector.x = (float)(sin(Angle) * Scale);
	FinalVector.y = Front.y;
	FinalVector.z = (float)(cos(Angle) * Scale);
	
	return FinalVector;
}

int UnsignedToSigned(unsigned int x)
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

Vector GetVectorFromPath(Name path, int point = 0)
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

Handle ReplaceObject(Handle h, char* odf, bool keepHealth = false)
{
	char TempMsgString[1024];

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


RebuildMission::RebuildMission(void)
{
	b_count = &b_last - &b_first - 1;
	b_array = &b_first + 1;

	f_count = &f_last - &f_first - 1;
	f_array = &f_first + 1;

	h_count = &h_last - &h_first - 1;
	h_array = &h_first + 1;

	i_count = &i_last - &i_first - 1;
	i_array = &i_first + 1;

	//v_count = &v_last - &v_first - 1;
	//v_array = &v_first + 1;
}

void RebuildMission::Init(void)
{
	if(i_array)
		memset(i_array, 0, i_count*sizeof(int));
	if(f_array)
		memset(f_array, 0, f_count*sizeof(float));
	if(h_array)
		memset(h_array, 0, h_count*sizeof(Handle));
	if(b_array)
		memset(b_array, 0, b_count*sizeof(bool));
//	if(v_array)
//		memset(v_array, 0, v_count*sizeof(Vector));

	EnableHighTPS(m_GameTPS);

	m_DidOneTimeInit = true;
}
void RebuildMission::Execute(void)
{
	if(!m_DidOneTimeInit)
		Init();

	PortalFunctions();
}
EjectKillRetCodes RebuildMission::PlayerEjected(void)
{
	return DoEjectPilot;
}

EjectKillRetCodes RebuildMission::ObjectKilled(int DeadObjectHandle,int KillersHandle)
{
	//if(!IsPlayer(DeadObjectHandle)) // J.Random AI-controlled Object is toast
	// return DLLHandled;
	//else // Player dead
	return DoEjectPilot;
}
bool RebuildMission::Load(bool missionSave)
{
	EnableHighTPS(m_GameTPS);

	if (missionSave) {
		int i;

		// init bools
		for (i = 0; i < b_count; i++)
			b_array[i] = false;

		// init floats
		for (i = 0; i < f_count; i++)
			f_array[i] = 99999.0f;

		// init handles
		for (i = 0; i < h_count; i++)
			h_array[i] = 0;

		// init ints
		for (i = 0; i < i_count; i++)
			i_array[i] = 0;

		m_ElapsedGameTime = 0;

		// Setup();
		return true;
	}

	bool ret = true;

	// bools
	if(b_count)
		Read(b_array, b_count);

	// floats
	if(f_count)
		Read(f_array, f_count);

	// Handles
	if(h_count)
		Read(h_array, h_count);

	// ints
	if(i_count)
		Read(i_array, i_count);

	Read(&m_ElapsedGameTime, 1);

	return ret;
}

bool RebuildMission::PostLoad(bool missionSave)
{
	if (missionSave)
	return true;

	bool ret = true;

	ConvertHandles(h_array, h_count);

	return ret;
}
bool RebuildMission::Save(bool missionSave)
{
	if (missionSave)
		return true;

	bool ret = true;

	// bools
	if(b_count)
	 Write(b_array, b_count);

	// floats
	if(f_count)
	 Write(f_array, f_count);

	// Handles
	if(h_count)
	 Write(h_array, h_count);

	// ints
	if(i_count)
	 Write(i_array, i_count);

	Read(&m_ElapsedGameTime, 1);

	return ret;
}

void RebuildMission::Wait(int &timer, int seconds)
{
	timer = m_ElapsedGameTime + (seconds * m_GameTPS);
}
bool RebuildMission::Move(Handle object, int degPerSec, int cmPerSec, Vector destination)
{
	//char TempMsgString[1024];

	if (!IsAround(object)) return false;
	bool Waiting = true;

	Vector currentPos = Vector();
	GetPosition(object, currentPos);

	if(degPerSec > 0)
	{
		Vector objectFront = GetFront(object);
		objectFront = HRotateFront(objectFront, degPerSec * CENTIGREES_TO_RADIANS / m_GameTPS);
		SetPosition(object, Build_Directinal_Matrix(currentPos, objectFront));
	}

	Vector ObjectToDestVector = Vector(destination.x - currentPos.x,destination.y - currentPos.y,destination.z - currentPos.z);
	//float Speed = cmPerSec * 0.01f;
	//float MPS = cmPerSec * 0.001f;
	float MPS = cmPerSec * 0.1f;
	float Speed = MPS / static_cast<float>(m_GameTPS);

	//sprintf(TempMsgString,"MPS: %f Speed: %f", MPS, Speed);
	//PrintConsoleMessage(TempMsgString);

	if (((ObjectToDestVector.x * ObjectToDestVector.x) +
	(ObjectToDestVector.y * ObjectToDestVector.y) +
	(ObjectToDestVector.z * ObjectToDestVector.z)) <= (Speed * Speed))
	{
		Waiting = false;
	}

	// Normalize the difference vector, and move the object along it according to
	// the speed variable indexed by Item3.
	ObjectToDestVector = Normalize_Vector(ObjectToDestVector);
	currentPos.x = currentPos.x + (ObjectToDestVector.x * Speed);
	currentPos.y = currentPos.y + (ObjectToDestVector.y * Speed);
	currentPos.z = currentPos.z + (ObjectToDestVector.z * Speed);
	SetVectorPosition(object, currentPos);

	//sprintf(TempMsgString,"Applying Move %f %f %f",(ObjectToDestVector.x * Speed),(ObjectToDestVector.y * Speed),(ObjectToDestVector.z * Speed));
	//PrintConsoleMessage(TempMsgString);

	ObjectToDestVector.x *= MPS;
	ObjectToDestVector.y *= MPS;
	ObjectToDestVector.z *= MPS;
	//SetVelocity(object, ObjectToDestVector);

	//sprintf(TempMsgString,"Applying Veloc %f %f %f",ObjectToDestVector.x,ObjectToDestVector.y,ObjectToDestVector.z);
	//PrintConsoleMessage(TempMsgString);

	return Waiting;
}

void RebuildMission::SetPortal(Handle object)
{
	for (int i=0; i<MaxPortals; i++)
	{
		if(!IsAround(Portals[i]))
		{
			Portals[i] = object;
			PortalCount++;
			int j;
			int k = i;

			for (j=0; j<PortalCount-1; j++)
			{
				if (++k >= PortalCount)
				{
					k = 0;
				}

				if(IsAround(Portals[k]) && GetTeamNum(Portals[k]) == GetTeamNum(Portals[i]))
				{
					PortalsDest[i] = Portals[k];
					break;
				}
			}

			k = i;

			for (j=0; j<PortalCount-1; j++)
			{
				if (--k < 0)
				{
					k = PortalCount-1;
				}

				if(IsAround(Portals[k]) && GetTeamNum(Portals[k]) == GetTeamNum(Portals[i]))
				{
					PortalsDest[k] = Portals[i];
					break;
				}
			}

			break;
		}
		else
		{
			if (i >= MaxPortals - 1)
			{
				SetObjectiveName(object, "Useless");
			}
		}
	}
}

void RebuildMission::SetPortalDest(Handle input, Handle output)
{
	for (int i=0; i<MaxPortals; i++)
	{
		if (input == Portals[i])
		{
			PortalsDest[i] = output;
			break;
		}
	}
}
//  This function regularly checks for proximity to a portal, and executes
// the set routine if true.  Function by B.S.
// _________________________________________________________________
void RebuildMission::PortalFunctions()
{
	for (int i=0; i < LabelledPortalCount; i++)
	{
		if (IsAround(LabelledPortals[i]))
		{
			int PortalTeam = GetTeamNum(LabelledPortals[i]);
			Handle NearVehicle = GetNearestObject(LabelledPortals[i]);
			if (IsWithin(NearVehicle,LabelledPortals[i], 14))
			{
				Vector PortalPos;
				Vector ObjectPos;
				Vector Front;

				GetPosition(NearVehicle, ObjectPos);
				GetPosition(LabelledPortals[i], PortalPos);

				if (ObjectPos.y < PortalPos.y + 7.0)
				{
					int j = i;

					for (int k=0; k<LabelledPortalCount-1; k++)
					{
						if (++j >= LabelledPortalCount)
						{
							j=0;
						}

						if ((PortalGroup[i] == PortalGroup[j]) && IsAround(LabelledPortals[j]))
						{
							int NearVehicleTeam = GetTeamNum(NearVehicle);

							GetPosition(LabelledPortals[j], PortalPos);

							GetFront(NearVehicle, Front);
							PortalPos.x += Front.x * 30.0f;
							PortalPos.z += Front.z * 30.0f;

							Front.x *= 10.0f;
							Front.z *= 10.0f;

							if ((PortalTeam == 0) || (PortalTeam == NearVehicleTeam) ||
								IsAlly(NearVehicle, LabelledPortals[i]))
							{
								SetVectorPosition(NearVehicle, PortalPos);



								if (IsPlayer(NearVehicle))
								{
									SetColorFade(1.0,1.0,32767);
									StartSoundEffect("teleport.wav", 0);
									AddHealth(NearVehicle, -15);
								}
								else
								{
									BuildObject("TeleportOut", 0, PortalPos);
									BuildObject("TeleportIn", 0, ObjectPos);
									Stop(NearVehicle,0);
								}
							}
							else
							{
								Front.x *= -3;
								Front.z *= -3;
							}
							SetVelocity(NearVehicle, Front);

							k = LabelledPortalCount; // Force this loop to terminate
						}
					}
				}
			}
		}
	}

	for (int i=0; i < PortalCount; i++)
	{
		if (IsAround(Portals[i]))
		{
			Handle NearVehicle = GetNearestVehicle(Portals[i]);
			
			Vector NearVehiclePos;
			Vector PortalPos;

			GetPosition(NearVehicle, NearVehiclePos);
			GetPosition(Portals[i], PortalPos);

			if (NearVehiclePos.y <= PortalPos.y + 7.0)
			{
				OnPortalProximity(Portals[i], NearVehicle);
			}
		}
	}
}

void RebuildMission::OnPortalProximity(Handle Portal, Handle NearVehicle)
{
	PrintConsoleMessage("Portal Check");
}
Handle RebuildMission::GetPortalDest(Handle Portal)
{
	for (int i=0; i<MaxPortals; i++)
	{
		if (Portal == Portals[i])
		{
			return PortalsDest[i];
		}
	}
	return 0;
}
void RebuildMission::Teleport(Handle object, Handle destination, int overshoot)
{
	Vector PortalPos;
	Vector Front;
	Vector StartPos;

	GetPosition(object, StartPos);
	
	GetPosition(destination, PortalPos);
	GetFront(object, Front);

	PortalPos.x += Front.x * (float)overshoot;
	PortalPos.z += Front.z * (float)overshoot;

	Front.x *= 8.0f;
	Front.z *= 8.0f;

	if (IsPlayer(object))
	{
		SetColorFade(1.0,1.0,32767);
		StartSoundEffect("teleport.wav", 0);
		AddHealth(object, -15);
	}
	else
	{
		BuildObject("TeleportOut", 0, PortalPos);
		BuildObject("TeleportIn", 0, StartPos);
		Stop(object, 0);
	}
	
	SetVectorPosition(object, PortalPos);
	SetVelocity(object, Front);
}
int RandomInt(int range)
{
	int retVal = (int)GetRandomFloat((float)(range));
	if(retVal == range) return (range - 1);
	return retVal;
}
#endif // _RebuildMission_20140119_
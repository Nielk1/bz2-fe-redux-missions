#include "RebuildMission.h"
#include <math.h>

RebuildMission *mission;

EjectKillRetCodes RebuildMission::PlayerEjected(void)
{
	return DoEjectPilot;
}

EjectKillRetCodes RebuildMission::ObjectKilled(int DeadObjectHandle,int KillersHandle)
{
	//if(!IsPlayer(DeadObjectHandle)) // J.Random AI-controlled Object is toast
	//	return DLLHandled;
	//else // Player dead
		return DoEjectPilot;
}

RebuildMission::RebuildMission(void)
{
	/*b_array = NULL;
	b_count = 0;

	f_array = NULL;
	f_count = 0;

	h_array = NULL;
	h_count = 0;

	i_array = NULL;
	i_count = 0;*/
}


bool RebuildMission::Load(bool missionSave)
{
	EnableHighTPS(m_GameTPS);

	if (missionSave) {
//		int i;

		// init bools
//		for (i = 0; i < b_count; i++)
//			b_array[i] = false;

		// init floats
//		for (i = 0; i < f_count; i++)
//			f_array[i] = 99999.0f;

		// init handles
//		for (i = 0; i < h_count; i++)
//			h_array[i] = 0;

		// init ints
//		for (i = 0; i < i_count; i++)
//			i_array[i] = 0;

		m_ElapsedGameTime = 0;

//		Setup();
		return true;
	}

	bool ret = true;

	// bools
//	if(b_count)
//		Read(b_array, b_count);

	// floats
//	if(f_count)
//		Read(f_array, f_count);

	// Handles
//	if(h_count)
//		Read(h_array, h_count);

	// ints
//	if(i_count)
//		Read(i_array, i_count);

	Read(&m_DidOneTimeInit, 1);
	Read(&m_ElapsedGameTime, 1);

	return ret;
}

/*bool RebuildMission::PostLoad(bool missionSave)
{
	if (missionSave)
		return true;

	bool ret = true;

	//ConvertHandles(h_array, h_count);

	return ret;
}*/

bool RebuildMission::Save(bool missionSave)
{
	if (missionSave)
		return true;

	bool ret = true;

	// bools
	//if(b_count)
	//	Write(b_array, b_count);

	// floats
	//if(f_count)
	//	Write(f_array, f_count);

	// Handles
	//if(h_count)
	//	Write(h_array, h_count);

	// ints
	//if(i_count)
	//	Write(i_array, i_count);

	Read(&m_DidOneTimeInit, 1);
	Read(&m_ElapsedGameTime, 1);

	return ret;
}

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

Vector GetVectorFromPath(Name path, int point)
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

Handle ReplaceObject(Handle h, char* odf, bool keepHealth)
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

void RebuildMission::Wait(int &timer, int seconds)
{
	timer = m_ElapsedGameTime + (seconds * m_GameTPS);
}

bool RebuildMission::Move(Handle object, int degPerSec, int cmPerSec, Vector destination)
{
	char TempMsgString[1024];

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
	float MPS = cmPerSec * 0.01f;
	float Speed = MPS / static_cast<float>(m_GameTPS);

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

	ObjectToDestVector.x *= MPS;
	ObjectToDestVector.y *= MPS;
	ObjectToDestVector.z *= MPS;
	SetVelocity(object, ObjectToDestVector);

	return Waiting;
}
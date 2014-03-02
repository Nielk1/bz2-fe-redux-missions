#ifndef _ScriptEngine_H_20140119_
#define _ScriptEngine_H_20140119_

#pragma once

#include "DLLBase.h"
#include <math.h>

#ifndef _ScripUtils_
#  error Please include the "ScriptUtils.h" file *before* including this file
#endif






// This determines if operation data is sent to the console in the debug version.  Set by
// the runspeed command, but only used with the debug version.  Original initial values
// specified in the script will be used if dubugging a saved game for whatever reason.
//bool DebugText[MaxRoutineCount] = {true, true, true, true, true, true, true, true, true, true,
//								   true, true, true, true, true, true, true, true, true, true,
//#ifdef LARGE_DLL
//								   true, true, true, true, true, true, true, true, true, true
//#endif


#define _DEBUG_CMD_STR1(a, b, c, d, e, f  ) {}
#define _DEBUG_CMD_STR2( a, b, c, d, e, f ) {}
#define _DEBUG_CMD_STR3( a, b, c, d, e, f ) {}
#define _DEBUG_CMD_STR4( a, b, c, d, e, f ) {}
#define _DEBUG_CMD_INT(a, b, c, d, e, f  ) {}






#define RandomRange 0.999999f
#define Interval 0.10f // time interval between DLL calls

#ifdef LARGE_DLL
#define OperationCount 1400
#define MaxRoutineCount 30
#define PosCount 48
#define IntegerCount 48
#define HandleCount 96
#define StringCount 200
#define ObjectiveCount 25
#else
#define OperationCount 1000
#define MaxRoutineCount 20
#define PosCount 32
#define IntegerCount 32
#define HandleCount 64
#define StringCount 150
#define ObjectiveCount 15
#endif


struct StepData
{
	short Instruction; // The switch-case branch that this step will use.
	short Item1; // These depend on the function used for this step.  They can be
	short Item2; // indices to handles, vectors, strings etc., or for other information.
	short Item3;
	short Item4;

	StepData () {}

	StepData(const short vInstruction,const short vItem1,const short vItem2,const short vItem3,const short vItem4)
		:Instruction(vInstruction),Item1(vItem1),Item2(vItem2),Item3(vItem3),Item4(vItem4)
	{
	}
};



























#define MaxPortals 20
#define MaxLabelledPortals 12
#define DEGREES_TO_RADIANS 0.01745329f
#define CENTIGREES_TO_RADIANS 0.0001745329f
#define RADIANS_TO_DEGREES 57.2957877f
#define PI 3.1415927f
#define TWO_PI 6.2831854f

#define MaxObjectives 10

#define TURQUOISE RGB(0,255,255)
#define YELLOW RGB(255,255,0)
#define BLUE RGB(0,0,255)
#define VIOLET RGB(255,0,255)

static const VehicleControls Controls = {0.0f, 0.0f, 0.0f, 0.0f, -128, -128, -128, -128, -128};

//////////////////////////////////////////////////////////////////////////
// This class requires a few things from the parent class, such as:
// * A non-private member variable of type int of name m_GameTPS
// * The ParentClass's most base baseclass must be DLLBase, or something
//   that exposes an identical interface.
// * ParentClass must be default constructible
//////////////////////////////////////////////////////////////////////////
template
<
	class ParentClass //: public DLLBase
>
class ScriptEngine : public ParentClass
{
	// Normally this next line is used and base_class is used everywhere
	// instead of ParentClass for compatibility with older compilers, but
	// since the BZ2 mission source seems to mandate a VC2k3 at minimum
	// anyway, no need anymore... can delete the next line and this
	// paragraph
	// typedef ParentClass base_class;

private:
//	std::vector<detail::loopQueue> m_WaitingLoops;

	//////////////////////////////////////////////////////////////////////
	// Main interface and code
public:
	// bools that need saving with the game
	bool
		b_first,
		RoutineWaiting[MaxRoutineCount],
		RoutinePriority[MaxRoutineCount],
		SavePriority,
		CameraCanCut,
		FirstFrame,
		b_last;

	// floats that need saving with the game
	float f_first;
	// Vectors 
	Vector VectorArray[PosCount];
	Vector RoutinePos[MaxRoutineCount];
	Vector RoutineFront[MaxRoutineCount];
	Vector CameraVector;
	Vector CameraDiffVector;
	float f_last;

	// handles that need saving with the game
	Handle h_first;
	Handle HandleArray[HandleCount]; // Handles read in from the script
	Handle LabelledPortals[MaxLabelledPortals];
	Handle Portals[MaxPortals];
	Handle PortalsDest[MaxPortals];
	Handle HadeanWalker;
	Handle h_last;

	// integers that need saving with the game
	int
		i_first,
		IntegerArray[IntegerCount],
		RoutineStep[MaxRoutineCount],	
		RoutineSpeed[MaxRoutineCount],
		RoutineResult[MaxRoutineCount],
		LastManAtBat,
		MaxBatters,
		NewObjectRoutine,
		NewObjectIndex,
		NewObjectRunSpeed,
		ExObjectRoutine,
		ExObjectIndex,
		ExObjectRunSpeed,
		PortalRoutine,
		PortalIndex,
		PortalRunSpeed,
		PortalDist,
		PortalCount,
		SaveRoutineSpeed,
		PortalGroup[MaxLabelledPortals],
		LabelledPortalCount,
		ObjectiveStep[ObjectiveCount],
		m_DisplayTime,
		i_last;

	int RoutineCount; // Used locally in Execute(), but is also accessed for dubug in OpCodeMachine(...).
	int OpCount;
	bool m_bSaved;
	char StringSpace[200];
	char AudioSpace[40];

	ScriptEngine()
	{
		PrintConsoleMessage("BZS Script Engine Emulator\n");

		// Define overlay space for saving and loading games
		b_count = &b_last - &b_first - 1;
		b_array = &b_first + 1;

		f_count = &f_last - &f_first - 1;
		f_array = &f_first + 1;

		h_count = &h_last - &h_first - 1;
		h_array = &h_first + 1;

		i_count = &i_last - &i_first - 1;
		i_array = &i_first + 1;

		m_bSaved = true;
	}

	// Function by B.S.
	// _________________________________________________________________
	void Setup(void)
	{
		int i;

		// Preload all odfs for which the script author prefixed with '>'
		// Preload all audio messages for which the script author prefixed with '*'
		for (i=0; i < StringCount; i++)
		{
			if (NameArray[i][0] == '>')
			{
				PreloadODF(&NameArray[i][1]);
			}
			else if (NameArray[i][0] == '*')
			{
				PreloadAudioMessage(&NameArray[i][1]);
			}
		}
		
		// initialize the class data from the global data written to the DLL
		for (i=0; i < MaxRoutineCount; i++)
		{
			RoutineSpeed[i] = GlobalRoutineSpeed[i];
			RoutinePriority[i] = GlobalRoutinePriority[i];
			RoutineStep[i] = RoutineOffset[i];
			RoutineWaiting[i] = false;
		}

		for (i=0; i < sizeof(GlobalVectorArray)/sizeof(GlobalVectorArray[0]); i++)
		{
			VectorArray[i] = GlobalVectorArray[i];
		}

		for (i=0; i < sizeof(GlobalIntegerArray)/sizeof(GlobalIntegerArray[0]); i++)
		{
			IntegerArray[i] = GlobalIntegerArray[i];
		}

		for (i=0; i < MaxObjectives; i++)
		{
			ObjectiveStep[i] = -1;
		}

		LastManAtBat = -1;
		MaxBatters = 2;
		NewObjectRoutine = -1;
		ExObjectRoutine = -1;
		PortalDist = 0;
		PortalCount = 0;
		CameraCanCut = true;
		FirstFrame = true;
		m_bSaved = false;
		m_DisplayTime = 8;

		//PrintConsoleMessage("DLL created using the BZII DLL Scriptor 1.6 - © 2000, 2001.");
	}


	// The main function.  overrides the base class virtual function.
	// Function by B.S.
	// _________________________________________________________________
	virtual void Execute(void)
	{
		if (FirstFrame)
		{
			PortalInit();
			FirstFrame = false;
		}

		if (m_bSaved)
		{

			int TempDisplayTime = m_DisplayTime;
			m_DisplayTime = 0;

			ClearObjectives();
			for (int i=0; i < MaxObjectives; i++)
			{
				if (ObjectiveStep[i] >= 0)
				{
					int TempResult = 0;
					bool TempWaiting = false;
					OpCodeMachine
					( 
						RoutineData[ObjectiveStep[i]],
						ObjectiveStep[i],
						TempResult,
						TempWaiting
					);
				}
				else
				{
					break;
				}
			}

			m_bSaved = false;
			m_DisplayTime = TempDisplayTime;
		}

		PortalFunctions();

		int BatterCount = 0;

		RoutineCount = LastManAtBat+1;

		// Find the next active routine and perform its next operation.  This loop
		// makes sure you only search the list once, starting after the routine
		// that was serviced last.	Go through only the number of routines specified.
		for (int i=0; i<NumberOfRoutines; i++)
		{
			if (RoutineCount >= NumberOfRoutines)
			{
				RoutineCount = 0;
			}

			if (RoutineSpeed[RoutineCount] > 0)
			{	// If the routine is enabled...
				if (RoutinePriority[RoutineCount])
				{	// and is a high priority routine...
					for (OpCount=0; OpCount<RoutineSpeed[RoutineCount]; OpCount++)
					{
						// Call the OpCodeMachine function for the current routine's operation.
						OpCodeMachine
						( 
							RoutineData[RoutineStep[RoutineCount]],
							RoutineStep[RoutineCount],
							RoutineResult[RoutineCount],
							RoutineWaiting[RoutineCount]
						);
					}
				}
				else if (BatterCount < MaxBatters)
				{ // Otherwise, do the specified number of low priority routines.
					for (int OpCount=0; OpCount<RoutineSpeed[RoutineCount]; OpCount++)
					{
						// Call the OpCodeMachine function for the current routine's operation.
						OpCodeMachine
						( 
							RoutineData[RoutineStep[RoutineCount]],
							RoutineStep[RoutineCount],
							RoutineResult[RoutineCount],
							RoutineWaiting[RoutineCount]
						);
					}
					BatterCount++;
					LastManAtBat = RoutineCount;
				}
			}
			
			// Increment to process the next routine
			RoutineCount++;
		}

		ParentClass::Execute(); // Might need this first if we used the gametime, which we do not...
	}

	// This function receives the opcode and the operands of the routine's
	// current operation. It also receives the routine's 3 data registers;
	// Step, Result and Waiting.  It uses the set of 5 numbers generated by
	// the DLL Scriptor for each operation, one opcode and 4 operands.
	// Function by B.S.
	// _________________________________________________________________
	void OpCodeMachine( StepData &Input, int& Step, int& Result, bool& Waiting )
	{
		switch (Input.Instruction)
		{
		default: // Fall through to case 0.
		case 0: // This essentially stops the routine.
			RoutineSpeed[RoutineCount] = 0;
			Step--;
			break;

		case 1: // Create
			{
				_DEBUG_CMD_STR1(RoutineCount, "Create", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
				bool UseResult = false;
				for (int i=0; i<18; i++)
				{
					if (NameArray[Input.Item2][i] == '%')
					{
						UseResult = true;
						break;
					}
				}

				if (UseResult)
				{
					sprintf(StringSpace, NameArray[Input.Item2], Result);
				}
				else
				{
					strcpy(StringSpace, NameArray[Input.Item2]);
				}

				if (StringSpace[0] == '>')
				{
					HandleArray[Input.Item1] = 
					BuildObject (&StringSpace[1], Input.Item3, VectorArray[Input.Item4]);
				}
				else
				{
					HandleArray[Input.Item1] = 
						BuildObject (StringSpace, Input.Item3, VectorArray[Input.Item4]);
				}
			}
			break;

		case 2: // Wait
			if (Waiting)
			{
				if (GetTime() > Result)
				{
					Waiting = false;
				}
				else
				{
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_INT(RoutineCount, "Wait", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				Result = (int)GetTime() + Input.Item1;
				Waiting = true;
				Step--;
			}
			break;

		case 3: //Follow
			_DEBUG_CMD_INT(RoutineCount, "Follow", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Follow (HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 4: // Attack
			_DEBUG_CMD_INT(RoutineCount, "Attack", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Attack (HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 5: // Clear
			{
				_DEBUG_CMD_INT(RoutineCount, "Clear", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				ClearObjectives();
				OpCount--; // Since the next operation is usually to display text, make it occur
							// immediately so that the text doesn't blink.

				for (int i=0; i < MaxObjectives; i++)
				{
					ObjectiveStep[i] = -1;
				}
			}
			break;

		case 6: // Display
			{
				_DEBUG_CMD_INT(RoutineCount, "Display", Input.Item1, Input.Item2, IntegerArray[Input.Item3],IntegerArray[Input.Item4] )

				if (Input.Item3 >= 0)
				{
					if (Input.Item4 >= 0)
					{
						sprintf(StringSpace, ObjectiveArray[Input.Item1],IntegerArray[Input.Item3],IntegerArray[Input.Item4]);
					}
					else
					{
						sprintf(StringSpace, ObjectiveArray[Input.Item1],IntegerArray[Input.Item3]);
					}
				}
				else
				{
					sprintf(StringSpace, ObjectiveArray[Input.Item1]);
				}

				switch (Input.Item2)
				{
				case 0:
					AddObjective(StringSpace, WHITE, (float)m_DisplayTime);
					break;
				case 1:
					AddObjective(StringSpace, GREEN, (float)m_DisplayTime);
					break;
				case 2:
					AddObjective(StringSpace, RED, (float)m_DisplayTime);
					break;
				case 3:
					AddObjective(StringSpace, TURQUOISE, (float)m_DisplayTime);
					break;
				case 4:
					AddObjective(StringSpace, YELLOW, (float)m_DisplayTime);
					break;
				case 5:
					AddObjective(StringSpace, BLUE, (float)m_DisplayTime);
					break;
				case 6:
					AddObjective(StringSpace, VIOLET, (float)m_DisplayTime);
					break;
				}

				if (!m_bSaved)
				{
					for (int i=0; i < MaxObjectives; i++)
					{
						if (ObjectiveStep[i] != -1)
						{
							continue;
						}
						else
						{
							ObjectiveStep[i] = Step;
							break;
						}

					}
				}
			}
			break;

		case 7: // GetByTS
			_DEBUG_CMD_INT(RoutineCount, "GetByTS", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetObjectByTeamSlot(Input.Item2, Input.Item3);
			break;

		case 8: // Add
			_DEBUG_CMD_INT(RoutineCount, "Add", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IntegerArray[Input.Item1] + Input.Item2;
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			}
			break;

		case 9: // RunSpeed: to enable or disable a routine, and set how 
				// many operations are processed each time it's up to bat.
			_DEBUG_CMD_INT(RoutineCount, "RunSpeed", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )

			RoutineSpeed[Input.Item1] = Input.Item2;
			RoutinePriority[Input.Item1] = Input.Item3 > 0;
			DebugText[Input.Item1] = Input.Item4 > 0;
			break;

		case 10: // IfLT (IfLessThan)
			_DEBUG_CMD_INT(RoutineCount, "IfLT", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result < Input.Item1)
			{
				Step = Input.Item2-1;
			}
			break;

		case 11: // IfGT (IfGreaterThan)
			_DEBUG_CMD_INT(RoutineCount, "IfGT", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result > Input.Item1)
			{
				Step = Input.Item2-1;
			}
			break;

		case 12: // IfEQ (IfEqualTo)
			_DEBUG_CMD_INT(RoutineCount, "IfEQ", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result == Input.Item1)
			{
				Step = Input.Item2-1;
			}
			break;

		case 13: // IfNE (IfNotEqualTo)
			_DEBUG_CMD_INT(RoutineCount, "IfNE", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if ((int)Result != Input.Item1)
			{
				Step = Input.Item2-1;
			}
			break;

		case 14: // GetCommand
			_DEBUG_CMD_INT(RoutineCount, "GetCommand", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetCurrentCommand(HandleArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 15: // Set
			_DEBUG_CMD_INT(RoutineCount, "Set", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			IntegerArray[Input.Item1] = Input.Item2;
			break;

		case 16: // GoTo
			_DEBUG_CMD_STR1(RoutineCount, "GoTo", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			if (Input.Item4 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item4]);
				Goto(HandleArray[Input.Item1], StringSpace, Input.Item3);
			}
			else
			{
				Goto(HandleArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			}
			break;

		case 17: // IsAround
			_DEBUG_CMD_INT(RoutineCount, "IsAround", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IsAround(HandleArray[Input.Item1]);
			break;

		case 18: // Fail
			_DEBUG_CMD_INT(RoutineCount, "Fail", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item2 >= 0)
			{		
				FailMission(GetTime()+(float)Input.Item1,NameArray[Input.Item2]);
			}
			else
			{
				FailMission(GetTime()+(float)Input.Item1);
			}
			break;

		case 19: // Succeed
			_DEBUG_CMD_INT(RoutineCount, "Succeed", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item2 >= 0)
			{
				SucceedMission(GetTime()+(float)Input.Item1,NameArray[Input.Item2]);
			}
			else
			{
				SucceedMission(GetTime()+(float)Input.Item1);
			}
			break;

		case 20: // Stop
			_DEBUG_CMD_INT(RoutineCount, "Stop", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Stop(HandleArray[Input.Item1], Input.Item2);
			break;

		case 21: // DistObject
			_DEBUG_CMD_INT(RoutineCount, "Distobject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item1]))
			{
				Result = (int) GetDistance(HandleArray[Input.Item1],HandleArray[Input.Item2]);
			}
			else
			{
				Result = 100000;
			}
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			}
			break;

		case 22: // DistPath
				_DEBUG_CMD_STR1(RoutineCount, "DistPath", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item1]))
			{
				Result = (int) GetDistance(HandleArray[Input.Item1],NameArray[Input.Item2]);
			}
			else
			{
				Result = 100000;
			}
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			};
			break;

		case 23: // GetPlayer
			_DEBUG_CMD_INT(RoutineCount, "GetPlayer", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetPlayerHandle();
			Result = HandleArray[Input.Item1];
			break;

		case 24: // SetGroup
			_DEBUG_CMD_INT(RoutineCount, "SetGroup", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetGroup(HandleArray[Input.Item1], Input.Item2);
			break;

		case 25: // Createp (create at a path point)
			{
				_DEBUG_CMD_STR2(RoutineCount, "Createp", Input.Item1, NameArray[Input.Item2], Input.Item3, NameArray[Input.Item4] )
				bool UseResult = false;
				for (int i=0; i<18; i++)
				{
					if (NameArray[Input.Item2][i] == '%')
					{
						UseResult = true;
						break;
					}
				}

				if (UseResult)
				{
					sprintf(StringSpace, NameArray[Input.Item2], Result);
				}
				else
				{
					strcpy(StringSpace, NameArray[Input.Item2]);
				}

				if (StringSpace[0] == '>')
				{
					HandleArray[Input.Item1] = 
					BuildObject (&StringSpace[1], Input.Item3, NameArray[Input.Item4]);
				}
				else
				{
					HandleArray[Input.Item1] = 
						BuildObject (StringSpace, Input.Item3, NameArray[Input.Item4]);
				}
			}
			break;

		case 26: // Build
			_DEBUG_CMD_STR1(RoutineCount, "Build", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			if (Input.Item4 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2], IntegerArray[Input.Item4]);
			}
			else
			{
				strcpy(StringSpace, NameArray[Input.Item2]);
			}
			if (NameArray[Input.Item2][0] == '>')
			{
				Build(HandleArray[Input.Item1], &StringSpace[1], Input.Item3);
			}
			else
			{
				Build(HandleArray[Input.Item1], StringSpace, Input.Item3);
			}

			break;

		case 27: // JumpTo (Unconditional)
			_DEBUG_CMD_INT(RoutineCount, "JumpTo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Step = Input.Item1-1;
			break;

		case 28: // Defend
			_DEBUG_CMD_INT(RoutineCount, "Defend", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Defend2(HandleArray[Input.Item1],HandleArray[Input.Item2], Input.Item3);
			break;

		case 29: // IsOdf
			_DEBUG_CMD_STR1(RoutineCount, "IsODF", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			if (Input.Item3 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2], IntegerArray[Input.Item3]);
			}
			else
			{
				strcpy(StringSpace, NameArray[Input.Item2]);
			}
			if (StringSpace[0] == '>')
			{
				Result = IsOdf(HandleArray[Input.Item1], &StringSpace[1]);
			}
			else
			{
				Result = IsOdf(HandleArray[Input.Item1], StringSpace);
			}
			break;

		case 30: // StoreResult
			_DEBUG_CMD_INT(RoutineCount, "StoreResult", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			IntegerArray[Input.Item1] = Result;
			break;

		case 31: // SetStep
			_DEBUG_CMD_INT(RoutineCount, "SetStep", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			RoutineStep[Input.Item1] = Input.Item2;
			RoutineWaiting[Input.Item1] = false;
			break;

		case 32: // Audio
			if (Input.Item2 >= 0)
			{
				sprintf(AudioSpace, NameArray[Input.Item1],IntegerArray[Input.Item2]);
			}
			else
			{
				strcpy(AudioSpace, NameArray[Input.Item1]);
			}

			_DEBUG_CMD_STR3(RoutineCount, "Audio", AudioSpace, Input.Item2, Input.Item3, Input.Item4 )

			if (AudioSpace[0] == '*')
			{
				AudioMessage(&AudioSpace[1]);
			}
			else
			{
				AudioMessage(AudioSpace);
			}
			break;

		case 33: // BeaconOn
			_DEBUG_CMD_INT(RoutineCount, "BeaconOn", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetObjectiveOn(HandleArray[Input.Item1]);
			break;

		case 34: // BeaconOff
			_DEBUG_CMD_INT(RoutineCount, "BeaconOff", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetObjectiveOff(HandleArray[Input.Item1]);
			break;

		case 35: // NearObject
			_DEBUG_CMD_INT(RoutineCount, "NearObject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item2]))
			{
				HandleArray[Input.Item1] = GetNearestObject(HandleArray[Input.Item2]);
			}
			else
			{
				HandleArray[Input.Item1] = 0;
			}
			break;

		case 36: // NearVehicle
			_DEBUG_CMD_INT(RoutineCount, "NearVehicle", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item2]))
			{
				HandleArray[Input.Item1] = GetNearestVehicle(HandleArray[Input.Item2]);
			}
			else
			{
				HandleArray[Input.Item1] = 0;
			}
			break;

		case 37: // NearVehicleP
			_DEBUG_CMD_STR1(RoutineCount, "NearVehicleP", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			if (Input.Item4 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item4]);
				HandleArray[Input.Item1] = GetNearestVehicle(NameArray[Input.Item2], Input.Item3);
			}
			else
			{
				HandleArray[Input.Item1] = GetNearestVehicle(NameArray[Input.Item2], Input.Item3);
			}
			break;

		case 38: // NearBuilding
			_DEBUG_CMD_INT(RoutineCount, "NearBuilding", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item2]))
			{
				HandleArray[Input.Item1] = GetNearestBuilding(HandleArray[Input.Item2]);
			}
			else
			{
				HandleArray[Input.Item1] = 0;
			}
			Result = HandleArray[Input.Item1];
			break;

		case 39: //NearEnemy
			_DEBUG_CMD_INT(RoutineCount, "NearEnemy", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item2]))
			{
				HandleArray[Input.Item1] = GetNearestEnemy(HandleArray[Input.Item2]);
			}
			else
			{
				HandleArray[Input.Item1] = 0;
			}
			break;

		case 40: // ChangeSide
			_DEBUG_CMD_INT(RoutineCount, "ChangeSide", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			ChangeSide();
			break;

		case 41: // GetByLabel
			_DEBUG_CMD_STR1(RoutineCount, "GetByLabel", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			if (Input.Item3 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item3]);
				HandleArray[Input.Item1] = GetHandle(StringSpace);
			}
			else
			{
				HandleArray[Input.Item1] = GetHandle(NameArray[Input.Item2]);
			}
			break;

		case 42: // Service
			_DEBUG_CMD_INT(RoutineCount, "Service", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Service(HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 43: // Mine
			_DEBUG_CMD_STR1(RoutineCount, "Mine", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			Mine (HandleArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			break;

		case 44: // Patrol
			_DEBUG_CMD_STR1(RoutineCount, "Patrol", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			if (Input.Item4 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item4]);
				Patrol(HandleArray[Input.Item1], StringSpace, Input.Item3);
			}
			else
			{
				Patrol(HandleArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			}
			break;

		case 45: // GetIn
			_DEBUG_CMD_INT(RoutineCount, "GetIn", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			GetIn(HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 46: // Pickup
			_DEBUG_CMD_INT(RoutineCount, "Pickup", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Pickup(HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 47: // Dropoff
			_DEBUG_CMD_STR1(RoutineCount, "Dropoff", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			if (Input.Item4 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item4]);
				Dropoff(HandleArray[Input.Item1], StringSpace, Input.Item3);
			}
			else
			{
				Dropoff(HandleArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			}
			break;

		case 48: // LookAt
			_DEBUG_CMD_INT(RoutineCount, "LookAt", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			LookAt(HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 49: // AllLookAt
			_DEBUG_CMD_INT(RoutineCount, "AllLookAt", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AllLookAt (Input.Item1, HandleArray[Input.Item2], Input.Item3);
			break;

		case 50: // GetPos
			_DEBUG_CMD_INT(RoutineCount, "GetPos", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (RoutineCount == ExObjectRoutine)
			{
				GetPosition2(HandleArray[Input.Item1], VectorArray[Input.Item2]);
			}
			else
			{
				GetPosition(HandleArray[Input.Item1], VectorArray[Input.Item2]);
			}
			break;

		case 51: // HasPilot (and is around at all)
			_DEBUG_CMD_INT(RoutineCount, "HasPilot", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IsAliveAndPilot(HandleArray[Input.Item1]);
			break;

		case 52: // HealthPercent
			_DEBUG_CMD_INT(RoutineCount, "HealthPercent", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = (int) GetHealth(HandleArray[Input.Item1]) * 100;
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 53: // GetCurHealth
			_DEBUG_CMD_INT(RoutineCount, "GetCurHealth", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetCurHealth(HandleArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 54: // SetCurHealth
			_DEBUG_CMD_INT(RoutineCount, "SetCurHealth", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetCurHealth(HandleArray[Input.Item1], Input.Item2);
			break;

		case 55: // GetCurAmmo
			_DEBUG_CMD_INT(RoutineCount, "GetCurAmmo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetCurAmmo(HandleArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 56: // SetCurAmmo
			_DEBUG_CMD_INT(RoutineCount, "SetCurAmmo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetCurAmmo(HandleArray[Input.Item1], Input.Item2);
			break;

		case 57: // GetMaxHealth
			_DEBUG_CMD_INT(RoutineCount, "GetMaxHealth", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetMaxHealth(HandleArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 58: // SetMaxHealth
			_DEBUG_CMD_INT(RoutineCount, "SetMaxHealth", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetMaxHealth(HandleArray[Input.Item1], Input.Item2);
			break;

		case 59: // AddHealth
			_DEBUG_CMD_INT(RoutineCount, "AddHealth", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AddHealth(HandleArray[Input.Item1], Input.Item2);
			break;

		case 60: // AddAmmo
			_DEBUG_CMD_INT(RoutineCount, "AddAmmo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AddAmmo(HandleArray[Input.Item1], Input.Item2);
			break;

		case 61: // GetTeam
			_DEBUG_CMD_INT(RoutineCount, "GetTeam", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetTeamNum(HandleArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 62: // SetTeam
			_DEBUG_CMD_INT(RoutineCount, "SetTeam", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetTeamNum(HandleArray[Input.Item1], Input.Item2);
			break;

		case 63: // PlaceAtP
			_DEBUG_CMD_STR1(RoutineCount, "PlaceAtP", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			SetPosition(HandleArray[Input.Item1], NameArray[Input.Item2]);
			if (Input.Item3 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item3]);
				SetPosition(HandleArray[Input.Item1], StringSpace);
			}
			else
			{
				SetPosition(HandleArray[Input.Item1], NameArray[Input.Item2]);
			}
			break;

		case 64: // PlaceAt
			_DEBUG_CMD_INT(RoutineCount, "PlaceAt", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetVectorPosition(HandleArray[Input.Item1], VectorArray[Input.Item2]);
			break;

		case 65: // UnitSpeed
			{
				Vector TempPos;
				_DEBUG_CMD_INT(RoutineCount, "UnitSpeed", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				TempPos.x = (float)Input.Item2;
				TempPos.y = (float)Input.Item3;
				TempPos.z = (float)Input.Item4;
				SetVelocity(HandleArray[Input.Item1], TempPos);
			}
			break;

		case 66: // ShotBy
			_DEBUG_CMD_INT(RoutineCount, "ShotBy", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetWhoShotMe(HandleArray[Input.Item2]);
			break;

		case 67: // SetPTeam
			_DEBUG_CMD_INT(RoutineCount, "SetPTeam", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetPerceivedTeam(HandleArray[Input.Item1], Input.Item2);
			break;

		case 68: // SetPlan
			_DEBUG_CMD_STR3(RoutineCount, "SetPlan", NameArray[Input.Item1], Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item3 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item1],IntegerArray[Input.Item3]);
				SetPlan(StringSpace, Input.Item2);
			}
			else
			{
				SetPlan(NameArray[Input.Item1], Input.Item2);
			}
			break;

		case 69: // Ally
			_DEBUG_CMD_INT(RoutineCount, "Ally", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Ally(Input.Item1, Input.Item2);
			break;

		case 70: // UnAlly
			_DEBUG_CMD_INT(RoutineCount, "UnAlly", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			UnAlly(Input.Item1, Input.Item2);
			break;

		case 71: // StartTimer
			_DEBUG_CMD_INT(RoutineCount, "StartTimer", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			StartCockpitTimer(Input.Item1);
			break;

		case 72: // GetTimerTime
			_DEBUG_CMD_INT(RoutineCount, "GetTimerTime", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetCockpitTimer();
			break;

		case 73: // StopTimer
			_DEBUG_CMD_INT(RoutineCount, "StopTimer", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			StopCockpitTimer();
			break;

		case 74: // Quake
			_DEBUG_CMD_INT(RoutineCount, "Quake", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			StartEarthQuake((float)Input.Item1/100);
			break;

		case 75: // UpdateQuake
			_DEBUG_CMD_INT(RoutineCount, "UpdateQuake", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			UpdateEarthQuake((float)Input.Item1/100);
			break;

		case 76: // StopQuake
			_DEBUG_CMD_INT(RoutineCount, "StopQuake", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			StopEarthQuake();
			break;

		case 77: // ConsoleWord
			_DEBUG_CMD_STR3(RoutineCount, "ConsoleWord", NameArray[Input.Item1], Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item2 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item1],IntegerArray[Input.Item2]);
				PrintConsoleMessage(StringSpace);
			}
			else
			{
				PrintConsoleMessage(NameArray[Input.Item1]);
			}
			break;		

		case 78: // IsDeployed
			_DEBUG_CMD_INT(RoutineCount, "IsDeployed", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IsDeployed(HandleArray[Input.Item1]);
			break;

		case 79: // IsSelected
			_DEBUG_CMD_INT(RoutineCount, "IsSelected", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			IsSelected(HandleArray[Input.Item1]);
			break;

		case 80: // GiveWeapon
			_DEBUG_CMD_STR1(RoutineCount, "GiveWeapon", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			GiveWeapon(HandleArray[Input.Item1], NameArray[Input.Item2]);
			break;

		case 81: // EjectPilot
			_DEBUG_CMD_INT(RoutineCount, "EjectPilot", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			EjectPilot(HandleArray[Input.Item1]);
			break;

		case 82: // HopOut
			_DEBUG_CMD_INT(RoutineCount, "HopOut", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HopOut(HandleArray[Input.Item1]);
			break;

		case 83: // SetName
			_DEBUG_CMD_STR1(RoutineCount, "SetName", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4);
			if (Input.Item3 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item2],IntegerArray[Input.Item3]);
				SetObjectiveName(HandleArray[Input.Item1], StringSpace);
			}
			else
			{
				SetObjectiveName(HandleArray[Input.Item1], NameArray[Input.Item2]);
			}
			break;

		case 84: // ConsoleResult
			_DEBUG_CMD_INT(RoutineCount, "ConsoleResult", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
	  		sprintf(StringSpace, "Routine %i Result: %i", RoutineCount, Result);
			PrintConsoleMessage( StringSpace );
			break;

		case 85: // GlobalSpeed
			_DEBUG_CMD_INT(RoutineCount, "GlobalSpeed", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			MaxBatters = Input.Item1;
			break;

		case 86: // RandPos
			_DEBUG_CMD_INT(RoutineCount, "RandPos", Input.Item1, Input.Item2, Input.Item3, Input.Item4)
			Result = Input.Item2 + (int)(GetRandomFloat(RandomRange)*(Input.Item3 + 1 - Input.Item2));
			if (Result >= Input.Item2 && Result < MaxRoutineCount)
			{
				VectorArray[Input.Item1] = VectorArray[Result];
			}
			break;

		case 87: // RandJump
			_DEBUG_CMD_INT(RoutineCount, "RandJump", Input.Item1, Input.Item2, Input.Item3, Input.Item4)
			if (Input.Item4 >= 0)
			{
				Result = 5;
			}
			else if (Input.Item3 >= 0)
			{
				Result = 4;
			}
			else if (Input.Item2 >= 0)
			{
				Result = 3;
			}
			else if (Input.Item1 >= 0)
			{
				Result = 2;
			}
			else
			{
				Result = 1;
			}

			Result = (int)(Result*GetRandomFloat(RandomRange));

			switch (Result)
			{
			default:
			case 0:
				break;
			case 1:
				Step = Input.Item1-1;
				break;
			case 2:
				Step = Input.Item2-1;
				break;
			case 3:
				Step = Input.Item3-1;
				break;
			case 4:
				Step = Input.Item4-1;
			}
			break;

		case 88: // SetPos
			_DEBUG_CMD_INT(RoutineCount, "SetPos", Input.Item1, Input.Item2, Input.Item3, Input.Item4)
			VectorArray[Input.Item1].x = (float)Input.Item2;
			VectorArray[Input.Item1].y = (float)Input.Item3;
			VectorArray[Input.Item1].z = (float)Input.Item4;
			break;

		case 89: // CamObject
			if (Waiting)
			{
				if (GetTime() > Result || (CameraCanCut && CameraCancelled()))
				{
					CameraFinish();
					Waiting = false;
					RoutinePriority[RoutineCount]= SavePriority;
					RoutineSpeed[RoutineCount]= SaveRoutineSpeed;
				}
				else
				{
					CameraObject
					(
						HandleArray[Input.Item1],
						VectorArray[Input.Item2].x,
						VectorArray[Input.Item2].y,
						VectorArray[Input.Item2].z,
						HandleArray[Input.Item3]
					);
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_INT(RoutineCount, "CamObject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				CameraReady();
				Result = (int)GetTime() + Input.Item4;
				Waiting = true;
				SavePriority = RoutinePriority[RoutineCount];
				RoutinePriority[RoutineCount]= true;
				SaveRoutineSpeed = RoutineSpeed[RoutineCount];
				RoutineSpeed[RoutineCount]= 1;
				Step--;
			}
			break;

		case 90: // CamPath
			if (Waiting)
			{
				if (Result > 0)
				{
					CameraFinish();
					Waiting = false;
					RoutinePriority[RoutineCount]= SavePriority;
					RoutineSpeed[RoutineCount]= SaveRoutineSpeed;
				}
				else if (CameraCanCut && CameraCancelled())
				{
					CameraPathDir	// Calling this function reduces the lockup that would occur
									// if calling CameraCancelled() after CameraPath(...).
					(
						NameArray[Input.Item1],
						Input.Item2,
						Input.Item3
					);
					Result = 1;
					Step--;
				}
				else if
				(
					CameraPath
					(
						NameArray[Input.Item1],
						Input.Item2,
						Input.Item3,
						HandleArray[Input.Item4]
					)
				)
				{
					CameraFinish();
					Waiting = false;
					RoutinePriority[RoutineCount]= SavePriority;
					RoutineSpeed[RoutineCount]= SaveRoutineSpeed;
				}
				else
				{
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_STR3(RoutineCount, "CamPath", NameArray[Input.Item1], Input.Item2, Input.Item3, Input.Item4 )
				CameraReady();
				Waiting = true;
				Result = 0;
				SavePriority = RoutinePriority[RoutineCount];
				RoutinePriority[RoutineCount]= true;
				SaveRoutineSpeed = RoutineSpeed[RoutineCount];
				RoutineSpeed[RoutineCount]= 1;
				Step--;
			}
			break;

		case 91: // CamPathDir
			if (Waiting)
			{
				if
				(
					(Input.Item4 == 0 && (CameraCanCut && CameraCancelled()))
					|| CameraPathDir(NameArray[Input.Item1],Input.Item2,Input.Item3)
				)
				{
					CameraFinish();
					Waiting = false;
					RoutinePriority[RoutineCount]= SavePriority;
					RoutineSpeed[RoutineCount]= SaveRoutineSpeed;
				}
				else
				{
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_INT(RoutineCount, "CamPathDir", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				CameraReady();
				Waiting = true;
				SavePriority = RoutinePriority[RoutineCount];
				RoutinePriority[RoutineCount]= true;
				SaveRoutineSpeed = RoutineSpeed[RoutineCount];
				RoutineSpeed[RoutineCount]= 1;
				Step--;
			}
			break;

		case 92: // CamPos
			if (Waiting)
			{
				if (GetTime() > Result || (CameraCanCut && CameraCancelled()))
				{
					CameraFinish();
					Waiting = false;
				}
				else
				{
					// Compute the camera's next position and place the camera
					CameraObject
					(
						HandleArray[Input.Item1],
						CameraVector.x += CameraDiffVector.x,
						CameraVector.y += CameraDiffVector.y,
						CameraVector.z += CameraDiffVector.z,
						HandleArray[Input.Item1]
					);
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_INT(RoutineCount, "CamPos", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )

				// Start the camera at the first point
				CameraVector = VectorArray[Input.Item2];


				// Compute the difference vector between the starting and ending positions
				CameraDiffVector.x = VectorArray[Input.Item3].x - VectorArray[Input.Item2].x;
				CameraDiffVector.y = VectorArray[Input.Item3].y - VectorArray[Input.Item2].y;
				CameraDiffVector.z = VectorArray[Input.Item3].z - VectorArray[Input.Item2].z;

				// Compute the time it will take from the start to the end
				float Time = 100 * (float)sqrt(CameraDiffVector.x * CameraDiffVector.x +
					CameraDiffVector.y * CameraDiffVector.y + CameraDiffVector.z *
					CameraDiffVector.z)/Input.Item4;

				// Compute the incremental difference vector
				CameraDiffVector.x = CameraDiffVector.x * Interval / Time;
				CameraDiffVector.y = CameraDiffVector.y * Interval / Time;
				CameraDiffVector.z = CameraDiffVector.z * Interval / Time;

				// Compute the time when the camera shot must end
				Result = (int)(GetTime() + Time);

				CameraReady();
				Waiting = true;
				SavePriority = RoutinePriority[RoutineCount];
				RoutinePriority[RoutineCount]= true;
				SaveRoutineSpeed = RoutineSpeed[RoutineCount];
				RoutineSpeed[RoutineCount]= 1;
				Step--;
			}
			break;

		case 93: // GetTug
			_DEBUG_CMD_INT(RoutineCount, "GetTug", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetTug(HandleArray[Input.Item2]);
			break;

		case 94: // HasCargo
			_DEBUG_CMD_INT(RoutineCount, "HasCargo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = HasCargo(HandleArray[Input.Item1]);
			break;

		case 95: // AddScrap
			_DEBUG_CMD_INT(RoutineCount, "AddScrap", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AddScrap(Input.Item1, Input.Item2);
			break;

		case 96: // SetScrap
			_DEBUG_CMD_INT(RoutineCount, "SetScrap", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetScrap(Input.Item1, Input.Item2);
			break;

		case 97: // GetScrap
			_DEBUG_CMD_INT(RoutineCount, "GetScrap", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetScrap(Input.Item1);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 98: // GetMaxScrap
			_DEBUG_CMD_INT(RoutineCount, "GetMaxScrap", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetMaxScrap(Input.Item1);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 99: // GetTime
			_DEBUG_CMD_INT(RoutineCount, "GetTime", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = (int)GetTime();
			if (Input.Item1 >= 0)
			{
				IntegerArray[Input.Item1] = Result;
			}
			break;

		case 100: // TeamColor
			_DEBUG_CMD_INT(RoutineCount, "SetTeamColor", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetTeamColor(Input.Item1, Input.Item2, Input.Item3, Input.Item4);
			break;

		case 101: // ClearTeamColor
			_DEBUG_CMD_INT(RoutineCount, "ClearTeamColor", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			ClearTeamColor(Input.Item1);
			break;

		case 102: // GoToo (Go to object)
			_DEBUG_CMD_INT(RoutineCount, "GoToo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Goto(HandleArray[Input.Item1], HandleArray[Input.Item2], Input.Item3);
			break;

		case 103: // Remove
			_DEBUG_CMD_INT(RoutineCount, "Remove", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			RemoveObject(HandleArray[Input.Item1]);
			break;

		case 104: // IsPerson
			_DEBUG_CMD_INT(RoutineCount, "IsPerson", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IsPerson(HandleArray[Input.Item1]);
			break;

		case 105: // GetAlt
			{
				_DEBUG_CMD_INT(RoutineCount, "GetAlt", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				float Alt = TerrainFindFloor(VectorArray[Input.Item1].x, VectorArray[Input.Item1].z);
				Result = (int) Alt;
				if (Input.Item2 >= 0)
				{
					VectorArray[Input.Item2].x = VectorArray[Input.Item1].x;
					VectorArray[Input.Item2].y = Alt;
					VectorArray[Input.Item2].z = VectorArray[Input.Item1].z;
				}
			}
			break;

		case 106: // GetTarget
			_DEBUG_CMD_INT(RoutineCount, "GetTarget", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetCurrentWho(HandleArray[Input.Item2]);
			break;

		case 107: // KillPilot
			_DEBUG_CMD_INT(RoutineCount, "KillPilot", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			KillPilot(HandleArray[Input.Item1]);
			break;

		case 108: // HoppedOutOf
			_DEBUG_CMD_INT(RoutineCount, "HoppedOutOf", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = HoppedOutOf(HandleArray[Input.Item2]);
			break;

		case 109: // IsAlly
			_DEBUG_CMD_INT(RoutineCount, "IsAlly", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IsAlly(HandleArray[Input.Item1], HandleArray[Input.Item2]);
			break;

		case 110: // GetMyForce: small, medium or large
			_DEBUG_CMD_INT(RoutineCount, "GetMyForce", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetInstantMyForce();
			if (Input.Item1 >= 0)
			{
				IntegerArray[Input.Item1] = Result;
			}
			break;

		case 111: // GetCompForce: small, medium or large
			_DEBUG_CMD_INT(RoutineCount, "GetCompForce", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetInstantCompForce();
			if (Input.Item1 >= 0)
			{
				IntegerArray[Input.Item1] = Result;
			}
			break;

		case 112: // GetMySide: 0 = Scion and 1 = ISDF
			_DEBUG_CMD_INT(RoutineCount, "GetMySide", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetInstantMySide();
			if (Input.Item1 >= 0)
			{
				IntegerArray[Input.Item1] = Result;
			}
			break;

		case 113: // GetMode: squad leader or commander
			_DEBUG_CMD_INT(RoutineCount, "GetMode", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetInstantType();
			if (Input.Item1 >= 0)
			{
				IntegerArray[Input.Item1] = Result;
			}
			break;

		case 114: // EvalPos
			_DEBUG_CMD_INT(RoutineCount, "EvalPos", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item2 == 0)
			{
				Result = (int)VectorArray[Input.Item1].x;
			}
			else if (Input.Item2 == 1)
			{
				Result = (int)VectorArray[Input.Item1].y;
			}
			else if (Input.Item2 == 2)
			{
				Result = (int)VectorArray[Input.Item1].z;
			}
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			}
			break;
			
		case 115: // Mult
			_DEBUG_CMD_INT(RoutineCount, "Mult", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IntegerArray[Input.Item1] * Input.Item2;
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			}
			break;

		case 116: // Fade
			_DEBUG_CMD_INT(RoutineCount, "Fade", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetColorFade((float)Input.Item1/100, (float)Input.Item2/100, Input.Item3);
			break;

		case 117: // AddToPos
			_DEBUG_CMD_INT(RoutineCount, "AddToPos", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			VectorArray[Input.Item4].x = VectorArray[Input.Item4].x + (float)Input.Item1;
			VectorArray[Input.Item4].y = VectorArray[Input.Item4].y + (float)Input.Item2;
			VectorArray[Input.Item4].z = VectorArray[Input.Item4].z + (float)Input.Item3;
			break;

		case 118: // RandShift
			_DEBUG_CMD_INT(RoutineCount, "RandShift", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			VectorArray[Input.Item4].x = VectorArray[Input.Item1].x + (float)Input.Item2*(GetRandomFloat(1.0f) - 0.5f);
			VectorArray[Input.Item4].y = VectorArray[Input.Item1].y;
			VectorArray[Input.Item4].z = VectorArray[Input.Item1].z + (float)Input.Item3*(GetRandomFloat(1.0f) - 0.5f);
			break;

		case 119: // PosAngle
			_DEBUG_CMD_INT(RoutineCount, "PosAngle", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetCircularPos
			(
				VectorArray[Input.Item1],
				(float)IntegerArray[Input.Item2],
				(float)IntegerArray[Input.Item3] * DEGREES_TO_RADIANS,
				VectorArray[Input.Item4]
			);
			break;

		case 120: // OnNewObject
			_DEBUG_CMD_INT(RoutineCount, "OnNewObject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item1 < 1)
			{
				NewObjectRoutine = -1;
			}
			else
			{
				NewObjectRunSpeed = Input.Item1;
				NewObjectRoutine = Input.Item2;
				NewObjectIndex = Input.Item3;
			}
			break;

		case 121: // OnDelObject
			_DEBUG_CMD_INT(RoutineCount, "OnDelObject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item1 < 1)
			{
				ExObjectRoutine = -1;
			}
			else
			{
				ExObjectRunSpeed = Input.Item1;
				ExObjectRoutine = Input.Item2;
				ExObjectIndex = Input.Item3;
			}
			break;

		case 122: // StartSound
			_DEBUG_CMD_STR3(RoutineCount, "StartSound", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			if (Input.Item2 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item1],IntegerArray[Input.Item2]);
			}
			else
			{
				strcpy(StringSpace, NameArray[Input.Item1]);
			}
			if (StringSpace[0] == '*')
			{
				Result = StartSoundEffect(&StringSpace[1]);
			}
			else
			{
				Result = StartSoundEffect(StringSpace);
			}
			break;

		case 123: // StopSound
			_DEBUG_CMD_INT(RoutineCount, "StopSound", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			StopSoundEffect(IntegerArray[Input.Item1]);
			break;

		case 124: // SetSkill
			_DEBUG_CMD_INT(RoutineCount, "SetSkill", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetSkill(HandleArray[Input.Item1], Input.Item2);
			break;

		case 125: // SetWeapon
			_DEBUG_CMD_INT(RoutineCount, "SetWeapon", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetWeaponMask(HandleArray[Input.Item1], (long)Input.Item2);
			break;

		case 126: // GetRace
			_DEBUG_CMD_INT(RoutineCount, "GetRace", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetRace(HandleArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
	#ifdef _BZDEBUG
			sprintf(StringSpace, "Race is: %c", (char)Result);
			PrintConsoleMessage(StringSpace);
	#endif
			break;

		case 127: // GetByIndex
			_DEBUG_CMD_INT(RoutineCount, "GetByIndex", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = Input.Item2 + IntegerArray[Input.Item3];
			if (Result < 50 && Result >= 0)
			{
				HandleArray[Input.Item1] = HandleArray[Result];
			}
			break;

		case 128: // SetByIndex
			_DEBUG_CMD_INT(RoutineCount, "SetByIndex", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = Input.Item1 + IntegerArray[Input.Item2];
			if (Result < HandleCount && Result >= 0)
			{
				HandleArray[Result] = HandleArray[Input.Item3];
			}
			break;

		case 129: // CamOf
			if (Waiting)
			{
				if ((CameraCanCut && CameraCancelled()) || !IsAround(HandleArray[Input.Item1]) || GetTime() > Result)
				{
					CameraFinish();
					Waiting = false;
					RoutinePriority[RoutineCount]= SavePriority;
					RoutineSpeed[RoutineCount]= SaveRoutineSpeed;
				}
				else
				{
					Vector ObjDir;
					Vector ObjPos;
					GetPosition(HandleArray[Input.Item1], ObjPos);
					GetFront(HandleArray[Input.Item1], ObjDir);
					ObjPos.x += ObjDir.x * Input.Item2;
					ObjPos.y += ObjDir.y * Input.Item2;
					ObjPos.z += ObjDir.z * Input.Item2;

					if (Input.Item4 >= 0)
					{
						ObjPos.x += VectorArray[Input.Item4].x;
						ObjPos.y += VectorArray[Input.Item4].y;
						ObjPos.z += VectorArray[Input.Item4].z;
					}

					SetCameraPosition(ObjPos, ObjDir);
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_INT(RoutineCount, "CamOf", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				CameraReady();
				Waiting = true;
				Result = (int)GetTime() + Input.Item3;
				SavePriority = RoutinePriority[RoutineCount];
				RoutinePriority[RoutineCount]= true;
				SaveRoutineSpeed = RoutineSpeed[RoutineCount];
				RoutineSpeed[RoutineCount]= 1;
				Step--;
			}
			break;

		case 130: //IsSameObject
			_DEBUG_CMD_INT(RoutineCount, "IsSameObject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = HandleArray[Input.Item1] == HandleArray[Input.Item2];
			break;

		case 131: // IsPlayer
			_DEBUG_CMD_INT(RoutineCount, "IsPlayer", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IsPlayer(HandleArray[Input.Item1]);
			break;

		case 132: // IFaceExec
			_DEBUG_CMD_STR3(RoutineCount, "IFaceExec", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_Exec(NameArray[Input.Item1]);
			break;

		case 133: // IFaceActivate
			_DEBUG_CMD_STR3(RoutineCount, "IFaceActivate", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_Activate(NameArray[Input.Item1]);
			break;

		case 134: // IFaceDeactivate
			_DEBUG_CMD_STR3(RoutineCount, "IFaceDeactivate", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_Deactivate(NameArray[Input.Item1]);
			break;

		case 135: // IFaceCommand
			_DEBUG_CMD_STR3(RoutineCount, "IFaceCommand", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_CreateCommand(NameArray[Input.Item1]);
			break;

		case 136: // IFaceString
			_DEBUG_CMD_STR4(RoutineCount, "IFaceString", NameArray[Input.Item1],  NameArray[Input.Item2], Input.Item3, Input.Item4)
			IFace_CreateString(NameArray[Input.Item1], NameArray[Input.Item2]);
			break;

		case 137: // IFaceSetString
			_DEBUG_CMD_STR4(RoutineCount, "IFaceSetString", NameArray[Input.Item1],  NameArray[Input.Item2], Input.Item3, Input.Item4)
			IFace_SetString(NameArray[Input.Item1], NameArray[Input.Item2]);
			break;

		case 138: // IFaceGetString
			_DEBUG_CMD_STR4(RoutineCount, "IFaceGetString", NameArray[Input.Item1],  NameArray[Input.Item2], Input.Item3, Input.Item4)
			IFace_GetString(NameArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			break;

		case 139: // IFaceInt
			_DEBUG_CMD_STR3(RoutineCount, "IFaceInt", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_CreateInteger(NameArray[Input.Item1], Input.Item2);
			break;

		case 140: // IFaceSetInt
			_DEBUG_CMD_STR3(RoutineCount, "IFaceSetInt", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_SetInteger(NameArray[Input.Item1], Input.Item2);
			break;

		case 141: // IFaceGetInt
			_DEBUG_CMD_STR3(RoutineCount, "IFaceGetInt", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			Result = IFace_GetInteger(NameArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 142: // IFaceClearList
			_DEBUG_CMD_STR3(RoutineCount, "IFaceClearList", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_ClearListBox(NameArray[Input.Item1]);
			break;

		case 143: // IFaceAddText
			_DEBUG_CMD_STR4(RoutineCount, "IFaceAddText", NameArray[Input.Item1],  NameArray[Input.Item2], Input.Item3, Input.Item4)
			IFace_AddTextItem(NameArray[Input.Item1], NameArray[Input.Item2]);
			break;

		case 144: // IFaceGetSelect
			_DEBUG_CMD_STR4(RoutineCount, "IFaceGetSelect", NameArray[Input.Item1],  NameArray[Input.Item2], Input.Item3, Input.Item4)
			IFace_GetSelectedItem(NameArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			break;

		case 145: // PlayMovie
			_DEBUG_CMD_STR3(RoutineCount, "PlayMovie", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			PlayMovie(NameArray[Input.Item1]);		
			break;

		case 146: // IsFlying
			_DEBUG_CMD_INT(RoutineCount, "IsFlying", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = (int)IsFlying(HandleArray[Input.Item1]);
			break;

		case 147: // Damage
			_DEBUG_CMD_INT(RoutineCount, "Damage", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Damage(HandleArray[Input.Item1], Input.Item2);
			break;

		case 148: // IsInfo
			_DEBUG_CMD_STR3(RoutineCount, "IsInfo", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			if (Input.Item2 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item1], IntegerArray[Input.Item2]);
			}
			else
			{
				strcpy(StringSpace, NameArray[Input.Item1]);
			}
			if (StringSpace[0] == '>')
			{
				Result = IsInfo(&StringSpace[1]);
			}
			else
			{
				Result = IsInfo(StringSpace);
			}
			break;
			
		case 149: // StartTimerUp
			_DEBUG_CMD_INT(RoutineCount, "StartTimerUp", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			StartCockpitTimerUp(Input.Item1);
			break;

		case 150: // SetAsPlayer
			_DEBUG_CMD_INT(RoutineCount, "SetAsPlayer", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetAsUser(HandleArray[Input.Item1], 1);
			break;

		case 151: // SetUserTarget
			_DEBUG_CMD_INT(RoutineCount, "SetUserTarget", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetUserTarget(HandleArray[Input.Item1]);
			break;

		case 152: // GetUserTarget
			_DEBUG_CMD_INT(RoutineCount, "GetUserTarget", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetUserTarget();
			break;

		case 153: // PlayBackVehicle
			_DEBUG_CMD_STR3(RoutineCount, "PlayBackVehicle", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			PlaybackVehicle(NameArray[Input.Item1]);
			break;

		case 154: // AddPilot
			_DEBUG_CMD_INT(RoutineCount, "AddPilot", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AddPilotByHandle(HandleArray[Input.Item1]);
			break;

		case 155: // SpawnBirds
			_DEBUG_CMD_STR2(RoutineCount, "SpawnBirds", Input.Item1, NameArray[Input.Item2], Input.Item3, NameArray[Input.Item4] )
			SpawnBirds(0, Input.Item1, NameArray[Input.Item2], Input.Item3, NameArray[Input.Item4]);
			break;

		case 156: // RemoveBirds
			_DEBUG_CMD_INT(RoutineCount, "RemoveBirds", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			RemoveBirds(0);
			break;

		case 157: // SetAnimation
			_DEBUG_CMD_STR1(RoutineCount, "SetAnimation", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			SetAnimation(HandleArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			break;

		case 158: // StartAnimation
			_DEBUG_CMD_STR1(RoutineCount, "StartAnimation", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			StartAnimation(HandleArray[Input.Item1]);
			break;

		case 159: // FreeCamera
			if (!Waiting)
			{
				_DEBUG_CMD_INT(RoutineCount, "FreeCamera", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				FreeCamera();
				if (Input.Item1 > 0)
				{
					Waiting = true;
					Step--;
				}
			}
			else
			{
				CameraReady();
				Waiting = false;
			}
			break;

		case 160: // FreeFinish
			if (!Waiting)
			{
				_DEBUG_CMD_INT(RoutineCount, "FreeFinish", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				FreeFinish();
				if (Input.Item1 > 0)
				{
					Waiting = true;
					Result = 0;
					Step--;
				}
			}
			else
			{
				if (++Result > 3)
				{
					CameraFinish();
					Waiting = false;
				}
				else
				{
					Step--;
				}
			}
			break;

		case 161: // SetPortal
			{
				_DEBUG_CMD_INT(RoutineCount, "SetPortal", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				for (int i=0; i<MaxPortals; i++)
				{
					if(!IsAround(Portals[i]))
					{
						Portals[i] = HandleArray[Input.Item1];
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
							SetObjectiveName(HandleArray[Input.Item1], "Useless");
						}
					}
				}
			}
			break;

		case 162: // GetPortalDest
			{
				_DEBUG_CMD_INT(RoutineCount, "GetPortalDest", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				HandleArray[Input.Item1] = 0;
				for (int i=0; i<MaxPortals; i++)
				{
					if (HandleArray[Input.Item2] == Portals[i])
					{
						HandleArray[Input.Item1] = PortalsDest[i];
						break;
					}
				}
			}
			break;

		case 163: // OnPortalDist
			_DEBUG_CMD_INT(RoutineCount, "OnPortalDist", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item1 < 1)
			{
				PortalRoutine = -1;
			}
			else
			{
				PortalRunSpeed = Input.Item1;
				PortalRoutine = Input.Item2;
				PortalIndex = Input.Item3;
				PortalDist = Input.Item4;
			}
			break;

		case 164: // Teleport
			{
				_DEBUG_CMD_INT(RoutineCount, "Teleport", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )

				Vector PortalPos;
				Vector Front;
				Vector StartPos;

				GetPosition(HandleArray[Input.Item1], StartPos);
				
				GetPosition(HandleArray[Input.Item2], PortalPos);
				GetFront(HandleArray[Input.Item1], Front);

				PortalPos.x += Front.x * (float)Input.Item3;
				PortalPos.z += Front.z * (float)Input.Item3;

				Front.x *= 8.0f;
				Front.z *= 8.0f;

				if (IsPlayer(HandleArray[Input.Item1]))
				{
					SetColorFade(1.0,1.0,32767);
					StartSoundEffect("teleport.wav");
					AddHealth(HandleArray[Input.Item1], -15);
				}
				else
				{
					BuildObject ("TeleportOut", 0, PortalPos);
					BuildObject ("TeleportIn", 0, StartPos);
					Stop(HandleArray[Input.Item1], 0);
				}
				
				SetVectorPosition(HandleArray[Input.Item1], PortalPos);
				SetVelocity(HandleArray[Input.Item1], Front);

			}
			break;

		case 165: // IfLTV (IfLessThan Variable)
			_DEBUG_CMD_INT(RoutineCount, "IfLTV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result < IntegerArray[Input.Item1])
			{
				Step = Input.Item2-1;
			}
			break;

		case 166: // IfGTV (IfGreaterThan)
			_DEBUG_CMD_INT(RoutineCount, "IfGTV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result > IntegerArray[Input.Item1])
			{
				Step = Input.Item2-1;
			}
			break;

		case 167: // IfEQV (IfEqualTo)
			_DEBUG_CMD_INT(RoutineCount, "IfEQV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result == IntegerArray[Input.Item1])
			{
				Step = Input.Item2-1;
			}
			break;

		case 168: // IfNEV (IfNotEqualTo)
			_DEBUG_CMD_INT(RoutineCount, "IfNEV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if ((int)Result != IntegerArray[Input.Item1])
			{
				Step = Input.Item2-1;
			}
			break;

		case 169: // Independence
			_DEBUG_CMD_INT(RoutineCount, "Independence", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetIndependence(HandleArray[Input.Item1], Input.Item2);
			break;

		case 170:  // NearObjectCount
			_DEBUG_CMD_INT(RoutineCount, "NearObjectCount", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = CountUnitsNearObject(HandleArray[Input.Item1], (float)IntegerArray[Input.Item2], Input.Item3, NameArray[Input.Item4]);
			break;

		case 171: // AddV
			_DEBUG_CMD_INT(RoutineCount, "AddV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IntegerArray[Input.Item1] + IntegerArray[Input.Item2];
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			}
			break;

		case 172: // MultV
			_DEBUG_CMD_INT(RoutineCount, "MultV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = IntegerArray[Input.Item1] * IntegerArray[Input.Item2];
			if (Input.Item3 >= 0)
			{
				IntegerArray[Input.Item3] = Result;
			}
			break;

		case 173: // Move
			{
				if (!Waiting)
				{
					_DEBUG_CMD_INT(RoutineCount, "Move", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )

					RoutinePos[RoutineCount] = GetPosition(HandleArray[Input.Item1]);
					RoutineFront[RoutineCount] = GetFront(HandleArray[Input.Item1]);
					RoutineSpeed[RoutineCount] = 1;
					RoutinePriority[RoutineCount] = true;
					Result = (int)GetTime();
					Waiting = true;
				}

				// Rotate the object
				RoutineFront[RoutineCount] = HRotateFront(RoutineFront[RoutineCount], IntegerArray[Input.Item2] * CENTIGREES_TO_RADIANS);
				SetPosition(HandleArray[Input.Item1], Build_Directinal_Matrix(RoutinePos[RoutineCount], RoutineFront[RoutineCount]));

				if (Input.Item4 >= 0)
				{	// Move the object if a destination was provided
					Vector ObjectToDestVector;

					// Get the difference vector between the object and the destination
					ObjectToDestVector.x = VectorArray[Input.Item4].x - RoutinePos[RoutineCount].x;
					ObjectToDestVector.y = VectorArray[Input.Item4].y - RoutinePos[RoutineCount].y;
					ObjectToDestVector.z = VectorArray[Input.Item4].z - RoutinePos[RoutineCount].z;

					// See if the object is about to go past the destination

					float Speed = IntegerArray[Input.Item3] * 0.01f;
					float MPS = IntegerArray[Input.Item3] * 0.001f;

					if (((ObjectToDestVector.x * ObjectToDestVector.x +
						ObjectToDestVector.y * ObjectToDestVector.y +
						ObjectToDestVector.z * ObjectToDestVector.z) <= Speed * Speed)
						|| !IsAround(HandleArray[Input.Item1]))
					{
						Waiting = false;
					}
					else
					{
						Step--;
					}

					// Normalize the difference vector, and move the object along it according to
					// the speed variable indexed by Item3.
					ObjectToDestVector = Normalize_Vector(ObjectToDestVector);
					RoutinePos[RoutineCount].x = RoutinePos[RoutineCount].x + ObjectToDestVector.x * Speed;
					RoutinePos[RoutineCount].y = RoutinePos[RoutineCount].y + ObjectToDestVector.y * Speed;
					RoutinePos[RoutineCount].z = RoutinePos[RoutineCount].z + ObjectToDestVector.z * Speed;
					SetVectorPosition(HandleArray[Input.Item1], RoutinePos[RoutineCount]);

					ObjectToDestVector.x *= MPS;
					ObjectToDestVector.y *= MPS;
					ObjectToDestVector.z *= MPS;
					SetVelocity(HandleArray[Input.Item1], ObjectToDestVector);
				}
				else
				{	// no destination provided, so set to current position
					SetVectorPosition(HandleArray[Input.Item1], RoutinePos[RoutineCount]);

					// Since no destination was provided, treat the 3rd argument as the time to completion
					// instead of speed to destination.

					if ((int)GetTime() >= Result + IntegerArray[Input.Item3])
					{
						Waiting = false;
					}
					else
					{
						Step--;
					}
				}
			}
			break;

		case 174: // SetTeamV
			_DEBUG_CMD_INT(RoutineCount, "SetTeamV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item2 >= 0)
			{
				SetTeamNum(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			}
			else if (Result >= 0 && Result < 16)
			{
				SetTeamNum(HandleArray[Input.Item1], Result);
			}
			break;

		case 175: // Replace
			{
				_DEBUG_CMD_STR1(RoutineCount, "Replace", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
				int Team = GetTeamNum(HandleArray[Input.Item1]);
				bool UseResult = false;
				Matrix M;
				GetPosition(HandleArray[Input.Item1], M);
				long Health = GetCurHealth(HandleArray[Input.Item1]);
				bool WasPlayer = IsPlayer(HandleArray[Input.Item1]);

				if (IsAround(HandleArray[Input.Item1]))
				{
					RemoveObject(HandleArray[Input.Item1]);
				}

				for (int i=0; i<18; i++)
				{
					if (NameArray[Input.Item2][i] == '%')
					{
						UseResult = true;
						break;
					}
				}

				if (UseResult)
				{
					sprintf(StringSpace, NameArray[Input.Item2], Result);
				}
				else
				{
					strcpy(StringSpace, NameArray[Input.Item2]);
				}

				if (StringSpace[0] == '>')
				{
					HandleArray[Input.Item1] = 
					BuildObject (&StringSpace[1], Team, M);
				}
				else
				{
					HandleArray[Input.Item1] = 
						BuildObject (StringSpace, Team, M);

					if (WasPlayer)
					{
						SetAsUser(HandleArray[Input.Item1], 1);
					}
				}

				if (Input.Item3 != 0)
				{
					SetCurHealth(HandleArray[Input.Item1], Health);
				}
			}
			break;

		case 176: // CameraCut
			_DEBUG_CMD_INT(RoutineCount, "CameraCut", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			CameraCanCut = Input.Item1 != 0;
			break;

		case 177: // SetAngle
			{
				_DEBUG_CMD_INT(RoutineCount, "SetAngle", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				Vector Front = Vector(0.0f, 0.0f, 1.0f);
				Front = HRotateFront(Front, Input.Item2 * CENTIGREES_TO_RADIANS);
				Vector Position = GetPosition(HandleArray[Input.Item1]);
				SetPosition(HandleArray[Input.Item1], Build_Directinal_Matrix(Position, Front));
				SetVectorPosition(HandleArray[Input.Item1], Position);
			}
			break;

		case 178: // UpdateQuakeV
			_DEBUG_CMD_INT(RoutineCount, "UpdateQuakeV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			UpdateEarthQuake((float)IntegerArray[Input.Item1]/100);
			break;

		case 179: // AddHealthV
			_DEBUG_CMD_INT(RoutineCount, "AddHealthV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AddHealth(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 180: // Divide
			_DEBUG_CMD_INT(RoutineCount, "Divide", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item2 != 0)
			{
				Result = IntegerArray[Input.Item1] / Input.Item2;
				if (Input.Item3 >= 0)
				{
					IntegerArray[Input.Item3] = Result;
				}
			}
			break;

		case 181: // AngleObject
			{
				_DEBUG_CMD_INT(RoutineCount, "AngleObject", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				Vector ObjPos;
				Vector SecondObjPos;
				GetPosition(HandleArray[Input.Item1], ObjPos);
				GetPosition(HandleArray[Input.Item2], SecondObjPos);
				Result = (int)(atan2(SecondObjPos.x - ObjPos.x, SecondObjPos.z - ObjPos.z) * RADIANS_TO_DEGREES);

				if (Input.Item3 >= 0)
				{
					IntegerArray[Input.Item3] = Result;
				}
			}
			break;

		case 182: // PosMultAdd
			_DEBUG_CMD_INT(RoutineCount, "PosMultAdd", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			VectorArray[Input.Item1].x = VectorArray[Input.Item2].x * Input.Item3;
			VectorArray[Input.Item1].y = VectorArray[Input.Item2].y * Input.Item3;
			VectorArray[Input.Item1].z = VectorArray[Input.Item2].z * Input.Item3;
			
			if (Input.Item4 >= 0)
			{
				VectorArray[Input.Item1].x += VectorArray[Input.Item4].x;
				VectorArray[Input.Item1].y += VectorArray[Input.Item4].y;
				VectorArray[Input.Item1].z += VectorArray[Input.Item4].z;
			}
			break;

		case 183: // SetPortalDest
			{
				_DEBUG_CMD_INT(RoutineCount, "SetPortalDest", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				for (int i=0; i<MaxPortals; i++)
				{
					if (HandleArray[Input.Item1] == Portals[i])
					{
						PortalsDest[i] = HandleArray[Input.Item2];
						break;
					}
				}
			}
			break;

		case 184: // Retreat
			_DEBUG_CMD_STR1(RoutineCount, "Retreat", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
			Retreat(HandleArray[Input.Item1], NameArray[Input.Item2], Input.Item3);
			break;

		case 185: // SetMaxAmmo
			_DEBUG_CMD_INT(RoutineCount, "SetMaxAmmo", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetMaxAmmo(HandleArray[Input.Item1], Input.Item2);
			break;

		case 186: // Deploy
			_DEBUG_CMD_INT(RoutineCount, "Deploy", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Deploy(HandleArray[Input.Item1]);
			break;

		case 187: // SetCurHealthV
			_DEBUG_CMD_INT(RoutineCount, "SetCurHealthV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetCurHealth(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 188: // SetCurAmmoV
			_DEBUG_CMD_INT(RoutineCount, "SetCurAmmoV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetCurAmmo(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 189: // GetEmptyGroup
			_DEBUG_CMD_INT(RoutineCount, "GetEmptyGroup", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = (int)GetFirstEmptyGroup();
			if (Input.Item1 >= 0)
			{
				IntegerArray[Input.Item1] = Result;
			}
			break;

		case 190: // PreloadAudio
			if (Input.Item2 >= 0)
			{
				sprintf(AudioSpace, NameArray[Input.Item1],IntegerArray[Input.Item2]);
			}
			else
			{
				strcpy(AudioSpace, NameArray[Input.Item1]);
			}

			_DEBUG_CMD_STR3(RoutineCount, "PreloadAudio", AudioSpace, Input.Item2, Input.Item3, Input.Item4 )

			if (AudioSpace[0] == '*')
			{
				PreloadAudioMessage(&AudioSpace[1]);
			}
			else
			{
				PreloadAudioMessage(AudioSpace);
			}
			break;

		case 191: // IFaceSetIntV
			_DEBUG_CMD_STR3(RoutineCount, "IFaceSetIntV", NameArray[Input.Item1],  Input.Item2, Input.Item3, Input.Item4)
			IFace_SetInteger(NameArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 192: // SetGroupV
			_DEBUG_CMD_INT(RoutineCount, "SetGroupV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetGroup(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 193: // SetMaxHealthV
			_DEBUG_CMD_INT(RoutineCount, "SetMaxHealthV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetMaxHealth(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 194: // SetMaxAmmoV
			_DEBUG_CMD_INT(RoutineCount, "SetMaxAmmoV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetMaxAmmo(HandleArray[Input.Item1], IntegerArray[Input.Item2]);
			break;

		case 195: // HideTimer
			_DEBUG_CMD_INT(RoutineCount, "HideTimer", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HideCockpitTimer();
			break;

		case 196: // AtTerminal
			_DEBUG_CMD_INT(RoutineCount, "AtTerminal", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item2]))
			{
				HandleArray[Input.Item1] = AtTerminal(HandleArray[Input.Item2]);
			}
			else
			{
				HandleArray[Input.Item1] = 0;
			}
			break;

		case 197: // InBuilding
			_DEBUG_CMD_INT(RoutineCount, "InBuilding", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (IsAround(HandleArray[Input.Item2]))
			{
				HandleArray[Input.Item1] = InBuilding(HandleArray[Input.Item2]);
			}
			else
			{
				HandleArray[Input.Item1] = 0;
			}
			break;

		case 198: // SetDisplayTime
			_DEBUG_CMD_INT(RoutineCount, "SetDisplayTime", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item1 >= 0)
			{
				m_DisplayTime = Input.Item1;
			}
			break;

		case 199: // DisplayFile
			{
				_DEBUG_CMD_INT(RoutineCount, "DisplayFile", Input.Item1, Input.Item2, IntegerArray[Input.Item3],IntegerArray[Input.Item4] )

				if (Input.Item3 >= 0)
				{
					if (Input.Item4 >= 0)
					{
						sprintf(StringSpace, NameArray[Input.Item1],IntegerArray[Input.Item3],IntegerArray[Input.Item4]);
					}
					else
					{
						sprintf(StringSpace, NameArray[Input.Item1],IntegerArray[Input.Item3]);
					}
				}
				else
				{
					sprintf(StringSpace, NameArray[Input.Item1]);
				}

				switch (Input.Item2)
				{
				case 0:
					AddObjective(StringSpace, WHITE, (float)m_DisplayTime);
					break;
				case 1:
					AddObjective(StringSpace, GREEN, (float)m_DisplayTime);
					break;
				case 2:
					AddObjective(StringSpace, RED, (float)m_DisplayTime);
					break;
				case 3:
					AddObjective(StringSpace, TURQUOISE, (float)m_DisplayTime);
					break;
				case 4:
					AddObjective(StringSpace, YELLOW, (float)m_DisplayTime);
					break;
				case 5:
					AddObjective(StringSpace, BLUE, (float)m_DisplayTime);
					break;
				case 6:
					AddObjective(StringSpace, VIOLET, (float)m_DisplayTime);
					break;
				}

				if (!m_bSaved)
				{
					for (int i=0; i < MaxObjectives; i++)
					{
						if (ObjectiveStep[i] != -1)
						{
							continue;
						}
						else
						{
							ObjectiveStep[i] = Step;
							break;
						}
					}
				}
			}
			break;

		case 200: // CreateV
			{
				_DEBUG_CMD_STR1(RoutineCount, "CreateV", Input.Item1, NameArray[Input.Item2], Input.Item3, Input.Item4 )
				bool UseResult = false;
				for (int i=0; i<18; i++)
				{
					if (NameArray[Input.Item2][i] == '%')
					{
						UseResult = true;
						break;
					}
				}

				if (UseResult)
				{
					sprintf(StringSpace, NameArray[Input.Item2], Result);
				}
				else
				{
					strcpy(StringSpace, NameArray[Input.Item2]);
				}

				if (StringSpace[0] == '>')
				{
					HandleArray[Input.Item1] = 
					BuildObject (&StringSpace[1], IntegerArray[Input.Item3], VectorArray[Input.Item4]);
				}
				else
				{
					HandleArray[Input.Item1] = 
						BuildObject (StringSpace, IntegerArray[Input.Item3], VectorArray[Input.Item4]);
				}
			}
			break;

		case 201: // CreatepV (create at a path point)
			{
				_DEBUG_CMD_STR2(RoutineCount, "CreatepV", Input.Item1, NameArray[Input.Item2], Input.Item3, NameArray[Input.Item4] )
				bool UseResult = false;
				for (int i=0; i<18; i++)
				{
					if (NameArray[Input.Item2][i] == '%')
					{
						UseResult = true;
						break;
					}
				}

				if (UseResult)
				{
					sprintf(StringSpace, NameArray[Input.Item2], Result);
				}
				else
				{
					strcpy(StringSpace, NameArray[Input.Item2]);
				}

				if (StringSpace[0] == '>')
				{
					HandleArray[Input.Item1] = 
					BuildObject (&StringSpace[1], IntegerArray[Input.Item3], NameArray[Input.Item4]);
				}
				else
				{
					HandleArray[Input.Item1] = 
						BuildObject (StringSpace, IntegerArray[Input.Item3], NameArray[Input.Item4]);
				}
			}
			break;

		case 202:  // NearObjectCountV
			_DEBUG_CMD_INT(RoutineCount, "NearObjectCountV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = CountUnitsNearObject(HandleArray[Input.Item1], (float)IntegerArray[Input.Item2], IntegerArray[Input.Item3], NameArray[Input.Item4]);
			break;

		case 203: // SetPlanV
			_DEBUG_CMD_STR3(RoutineCount, "SetPlanV", NameArray[Input.Item1], Input.Item2, Input.Item3, Input.Item4 )
			if (Input.Item3 >= 0)
			{
				sprintf(StringSpace, NameArray[Input.Item1],IntegerArray[Input.Item3]);
				SetPlan(StringSpace, Input.Item2);
			}
			else
			{
				SetPlan(NameArray[Input.Item1], IntegerArray[Input.Item2]);
			}
			break;

		case 204: // SetScrapV
			_DEBUG_CMD_INT(RoutineCount, "SetScrapV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			SetScrap(IntegerArray[Input.Item1], Input.Item2);
			break;

		case 205: // AddScrapV
			_DEBUG_CMD_INT(RoutineCount, "AddScrapV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			AddScrap(IntegerArray[Input.Item1], Input.Item2);
			break;

		case 206: // GetScrapV
			_DEBUG_CMD_INT(RoutineCount, "GetScrapV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetScrap(IntegerArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 207: // GetMaxScrapV
			_DEBUG_CMD_INT(RoutineCount, "GetMaxScrapV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			Result = GetMaxScrap(IntegerArray[Input.Item1]);
			if (Input.Item2 >= 0)
			{
				IntegerArray[Input.Item2] = Result;
			}
			break;

		case 208: // GetByTSV
			_DEBUG_CMD_INT(RoutineCount, "GetByTSV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			HandleArray[Input.Item1] = GetObjectByTeamSlot(IntegerArray[Input.Item2], Input.Item3);
			break;

		case 209: // WaitV
			if (Waiting)
			{
				if (GetTime() > Result)
				{
					Waiting = false;
				}
				else
				{
					Step--;
				}
			}
			else
			{
				_DEBUG_CMD_INT(RoutineCount, "WaitV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
				Result = (int)GetTime() + IntegerArray[Input.Item1];
				Waiting = true;
				Step--;
			}
			break;

		case 210: // LeaveScrap
			if (Input.Item2 != 0)
			{
				ClearNoScrapFlagByHandle(HandleArray[Input.Item1]);
			}
			else
			{
				SetNoScrapFlagByHandle(HandleArray[Input.Item1]);
			}
			break;

		case 211: // IfLE (IfLessThan or equal to)
			_DEBUG_CMD_INT(RoutineCount, "IfLE", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result <= Input.Item1)
			{
				Step = Input.Item2-1;
			}
			break;

		case 212: // IfGE (IfGreaterThan or equal to)
			_DEBUG_CMD_INT(RoutineCount, "IfGE", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result >= Input.Item1)
			{
				Step = Input.Item2-1;
			}
			break;

		case 213: // IfLEV (IfLessThan or equal to)
			_DEBUG_CMD_INT(RoutineCount, "IfLEV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result <= IntegerArray[Input.Item1])
			{
				Step = Input.Item2-1;
			}
			break;

		case 214: // IfGEV (IfGreaterThan or equal to)
			_DEBUG_CMD_INT(RoutineCount, "IfGEV", Input.Item1, Input.Item2, Input.Item3, Input.Item4 )
			if (Result >= IntegerArray[Input.Item1])
			{
				Step = Input.Item2-1;
			}
			break;
		}

		Step++;
	}



	// This function executes whenever an object is added to the mission,
	// and runs a routine that has been associated via the OnNewObject operation.
	// Function by B.S.
	// _________________________________________________________________
	virtual void AddObject(Handle NewObject)
	{
		if (NewObjectRoutine < 0 || RoutineSpeed[NewObjectRoutine] > 0)
		{
			return;
		}

		int TempRoutineCount = RoutineCount;
		int TempOpCount = OpCount;

		if (NewObjectIndex >= 0)
		{
			HandleArray[NewObjectIndex] = NewObject;
		}

		RoutineSpeed[NewObjectRoutine] = NewObjectRunSpeed;
		RoutineCount = NewObjectRoutine;

		for (int j=0; j<RoutineSpeed[RoutineCount]; j++)
		{
			// Call the OpCodeMachine function for the routine's operation.
			OpCodeMachine
			( 
				RoutineData[RoutineStep[RoutineCount]],
				RoutineStep[RoutineCount],
				RoutineResult[RoutineCount],
				RoutineWaiting[RoutineCount]
			);
		}

		RoutineCount = TempRoutineCount;
		OpCount = TempOpCount;

		ParentClass::AddObject(NewObject);
	}


	// This function executes whenever an object is removed from the mission,
	// and runs a routine that has been associated via the OnDelObject operation.
	// Function by B.S.
	// _________________________________________________________________
	virtual void DeleteObject(Handle ExObject)
	{
		if (ExObjectRoutine < 0 || RoutineSpeed[ExObjectRoutine] > 0)
		{
			return;
		}

		int TempRoutineCount = RoutineCount;
		int TempOpCount = OpCount;

		if (ExObjectIndex >= 0)
		{
			HandleArray[ExObjectIndex] = ExObject;
		}

		RoutineSpeed[ExObjectRoutine] = ExObjectRunSpeed;
		RoutineCount = ExObjectRoutine;

		for (OpCount=0; OpCount<RoutineSpeed[RoutineCount]; OpCount++)
		{
			// Call the OpCodeMachine function for the routine's operation.
			OpCodeMachine
			( 
				RoutineData[RoutineStep[RoutineCount]],
				RoutineStep[RoutineCount],
				RoutineResult[RoutineCount],
				RoutineWaiting[RoutineCount]
			);
		}

		RoutineCount = TempRoutineCount;
		OpCount = TempOpCount;

		ParentClass::DeleteObject(ExObject);
	}


	//  This function regularly checks for proximity to a portal, and executes
	// the set routine if true.  Function by B.S.
	// _________________________________________________________________
	void PortalFunctions()
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
										StartSoundEffect("teleport.wav");
										AddHealth(NearVehicle, -15);
									}
									else
									{
										BuildObject ("TeleportOut", 0, PortalPos);
										BuildObject ("TeleportIn", 0, ObjectPos);
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

		if (PortalRoutine < 0 || RoutineSpeed[PortalRoutine] > 0)
		{
			return;
		}

		for (int i=0; i < PortalCount; i++)
		{
			if (IsAround(Portals[i]))
			{
				Handle NearVehicle = GetNearestVehicle(Portals[i]);
				if (IsWithin(NearVehicle,Portals[i], (float)PortalDist))
				{
					if (PortalIndex >= 0)
					{
						HandleArray[PortalIndex] = Portals[i];
					}

					Vector NearVehiclePos;
					Vector PortalPos;

					GetPosition(NearVehicle, NearVehiclePos);
					GetPosition(Portals[i], PortalPos);

					if (NearVehiclePos.y <= PortalPos.y + 7.0)
					{
						RoutineSpeed[PortalRoutine] = PortalRunSpeed;
						RoutineCount = PortalRoutine;

						for (OpCount=0; OpCount<RoutineSpeed[RoutineCount]; OpCount++)
						{
							// Call the OpCodeMachine function for the routine's operation.
							OpCodeMachine
							( 
								RoutineData[RoutineStep[RoutineCount]],
								RoutineStep[RoutineCount],
								RoutineResult[RoutineCount],
								RoutineWaiting[RoutineCount]
							);
						}
					}
				}
			}
		}
	}


	// This function initializes all portals that are labelled PortalA0 ,
	// PortalB1 etc. up to PortalF9.
	// _________________________________________________________________
	void PortalInit()
	{
		char PortalLabel[10] = "PortalA0";
		Handle TempHandle;
		LabelledPortalCount = 0;

		for (int i=0; i<10; i++)
		{
			for (int j=0; j<6; j++)
			{
				PortalLabel[6] = 'A' + j;
				TempHandle = GetHandle(PortalLabel);
				if (IsAround(TempHandle))
				{
					LabelledPortals[LabelledPortalCount] = TempHandle;
					PortalGroup[LabelledPortalCount++] = j;
					if (LabelledPortalCount >= MaxLabelledPortals)
					{
						return;
					}
				}
			}
			PortalLabel[6] = 'A';
			PortalLabel[7]++;
		}
	}

	virtual bool Load(bool missionSave)
	{
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

			Setup();
			//return true;
			ParentClass::Load(missionSave);
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

		return ret && ParentClass::Load(missionSave);
	}

	virtual bool PostLoad(bool missionSave)
	{
		if (missionSave)
			//return true;
			return ParentClass::PostLoad(missionSave);

		bool ret = true;

		ConvertHandles(h_array, h_count);

		return ret & ParentClass::PostLoad(missionSave);
	}

	virtual bool Save(bool missionSave)
	{
		if (missionSave)
			//return true;
			return ParentClass::Save(missionSave);

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

		return ret & ParentClass::Save(missionSave);
	}

};

/*
#ifdef _BZDEBUG
#define _DEBUG_CMD_STR1( a, b, c, d, e, f ){CommandString1( a, b, c, d, e, f );}
#define _DEBUG_CMD_STR2( a, b, c, d, e, f ){CommandString2( a, b, c, d, e, f );}
#define _DEBUG_CMD_STR3( a, b, c, d, e, f ){CommandString3( a, b, c, d, e, f );}
#define _DEBUG_CMD_STR4( a, b, c, d, e, f ){CommandString4( a, b, c, d, e, f );}
#define _DEBUG_CMD_INT( a, b, c, d, e, f ){CommandInt( a, b, c, d, e, f );}
#else
#define _DEBUG_CMD_STR1(a, b, c, d, e, f  ) {}
#define _DEBUG_CMD_STR2( a, b, c, d, e, f ) {}
#define _DEBUG_CMD_STR3( a, b, c, d, e, f ) {}
#define _DEBUG_CMD_STR4( a, b, c, d, e, f ) {}
#define _DEBUG_CMD_INT(a, b, c, d, e, f  ) {}
#endif
*/
/*
void CommandString1(int Routine, char *Command, int i1, char* Name, int i2, int i3 )
{
	if (!DebugText[Routine])
		return;

	char szOut[60];

	sprintf(szOut, "Routine %i: %s,%i,\"%s\",%i,%i", Routine, Command, i1, Name, i2, i3);
	PrintConsoleMessage( szOut );
}

void CommandString2(int Routine, char *Command, int i1, char* Name, int i2, char* Name2 )
{
	if (!DebugText[Routine])
		return;

	char szOut[60];

	sprintf(szOut, "Routine %i: %s,%i,\"%s\",%i,\"%s\"", Routine, Command, i1, Name, i2, Name2);
	PrintConsoleMessage( szOut );
}

void CommandString3(int Routine, char *Command, char* Name, int i1, int i2, int i3)
{
	if (!DebugText[Routine])
		return;

	char szOut[60];

	sprintf(szOut, "Routine %i: %s,\"%s\",%i,%i,%i", Routine, Command, Name, i1, i2, i3);
	PrintConsoleMessage( szOut );
}

void CommandString4(int Routine, char *Command, char* Name1, char* Name2, int i1, int i2)
{
	if (!DebugText[Routine])
		return;

	char szOut[80];

	sprintf(szOut, "Routine %i: %s,\"%s\",\"%s\",%i,%i", Routine, Command, Name1, Name2, i1, i2);
	PrintConsoleMessage( szOut );
}

void CommandInt(int Routine, char *Command, int i1, int i2, int i3, int i4 )
{
	if (!DebugText[Routine])
		return;
	
	char szOut[60];

	sprintf(szOut, "Routine %i: %s,%i,%i,%i,%i", Routine, Command, i1, i2, i3, i4);
	PrintConsoleMessage( szOut );
}
*/

#endif //_ScriptEngine_H_20081205_
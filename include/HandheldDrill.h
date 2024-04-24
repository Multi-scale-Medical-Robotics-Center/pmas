/*
 ============================================================================
 Name : CPP_Template.h
 Author  :		Benjamin Spitzer
 Version :
 Description : 	GMAS C/C++ project header file for Template
 ============================================================================
 */

/*
 ============================================================================
 Project general functions prototypes
 ============================================================================
 */
void MainInit();
void MachineSequences();
void MainClose();
void MachineSequencesInit();
void EnableMachineSequencesTimer(int TimerCycle);
void BackgroundProcesses();
void MachineSequencesClose();
void MachineSequencesTimer(int iSig);
void ReadAllInputData();
void WriteAllOutputData();
int OnRunTimeError(const char *msg, unsigned int uiConnHndl,
		unsigned short usAxisRef, short sErrorID, unsigned short usStatus);
void TerminateApplication(int iSigNum);
void Emergency_Received(unsigned short usAxisRef, short sEmcyCode);
void ModbusWrite_Received();
int CallbackFunc(unsigned char* recvBuffer, short recvBufferSize, void* lpsock);
/*
 ============================================================================
 States functions
 ============================================================================
 */
void StateFunction_eA1Home(CMMCSingleAxis, int);
void StateFunction_eA2Start(CMMCSingleAxis, int);
void StateFunction_eRun();

/*
 ============================================================================
 General constants
 ============================================================================
 */
#define 	MAX_AXES				4		// number of Physical axes in the system. TODO Update MAX_AXES accordingly
#define 	FALSE					0
#define 	TRUE					1
//
// TODO: Modbus memory map offsets must be updated accordingly.
#define 	MODBUS_READ_OUTPUTS_INDEX	0	// Start of Modbus read address
#define 	MODBUS_READ_CNT				16	// Number of registers to read
#define 	MODBUS_UPDATE_START_INDEX	16	// Start of Modbus write address (update to host)
#define 	MODBUS_UPDATE_CNT			16	// Number of registers to update
/*
 ============================================================================
 Project constants
 ============================================================================
 */
// background thread
#define		SLEEP_TIME				2000	// Sleep time of the backround idle loop, in micro seconds
// Please ensure no re-entrancy
#define		TIMER_CYCLE_US			2000		// Cycle time of the main sequences timer, in us
/*
 ============================================================================
 States Machines constants
 ============================================================================
 */
#define 	FIRST_SUB_STATE			1
enum eMainStateMachines	// TODO: Change names of state machines to reflect dedicated project
{
	eIDLE = 0, eA1Home = 1,						// Main state machine #1
	eA2Start = 2,
	eA3Home = 3,
	eA4Start = 4,
	eRun = 5,						// Main state machine #2
	eError = 6,
};

enum eSubStateMachine_A1Home		// TODO: Change names of sub-state machines.
{
//	eSubState_SM_A1Home_PowerOff = 0,
//	eSubState_SM_A1Home_WaitOiwerOff = 1,
//	eSubState_SM_A1Home_OPModeHome =
	eSubState_SM_A1Home_PowerOff = 0,
	eSubState_SM_A1Home_WaitPowerOff = 1,
	eSubState_SM_A1Home_SetHomeMode = 2,
	eSubState_SM_A1Home_WaitHomeMode = 3,
	eSubState_SM_A1Home_PowerOn = 4,
	eSubState_SM_A1Home_WaitPowerOn = 5,
	eSubState_SM_A1Home_StartHoming = 6,
	eSubState_SM_A1Home_WaitHomingEnd = 7,
	eSubState_SM_A1Home_StartProfilePositionMode = 8,
	eSubState_SM_A1Home_WaitProfilePositionMode = 9,
	eSubState_SM_A1Home_GoZero = 10,
	eSubState_SM_A1Home_Done = 11,
	eSubState_SM_A1Home_Error = 12,
};
enum eSubStateMachine_A2Start		// TODO: Change names of sub-state machines.
{
	eSubState_SM_A2Start_PowerOff = 0,
	eSubState_SM_A2Start_WaitPowerOff = 1,
	eSubState_SM_A2Start_SetVelocityMode = 2,
	eSubState_SM_A2Start_WaitVelocityMode = 3,
	eSubState_SM_A2Start_PowerOn = 4,
	eSubState_SM_A2Start_WaitPowerOn = 5,
	eSubState_SM_A2Star_Done = 6,
	eSubState_SM_A2Start_Error = 7,
};
enum eSubStateMachine_Run			// TODO: Change names of sub-state machines.
{
	eSubState_SM_OK = 0, eSubState_SM_Error = 1,
};

/*
 ============================================================================
 Application global variables
 ============================================================================
 */
int giTerminate;		// Flag to request program termination
int giReentrance;		// Used to detect reentrancy to the main timer function

unsigned int giReentrance_counts = 0;
//
int giState1;			// Holds the current state of the 1st main state machine
int giPrevState1;		// Holds the value of giState1 at previous cycle
int giSubState1;// Holds teh current state of the sub-state machine of 1st main state machine

int giStateA1Home;		// Holds the current state of the 1st main state machine
int giPrevStateA1Home;		// Holds the value of giState1 at previous cycle
int giSubStateA1Home;// Holds teh current state of the sub-state machine of 1st main state machine
//
int giStateA2Start;
int giStateRun;			// Holds the current state of the 2nd main state machine
int giPrevStateRun;		// Holds the value of giState2 at previous cycle
int giSubStateRun;// Holds the current state of the sub-state machine of 2nd main state machine
//
// 	Examples for data read from the GMAS core about the X, Y drives
//int		giA1Position;
//int		giA2Position;
float gfA1Torque;
float gfA2Torque;
float gfA1Position;
float gfA2Velocity;
int giA1InMotion;
int giA2InMotion;
int giA1Status;
int giA2Status;
int giA3Status;
int giA4Status;

short gsA1AI;
short gsA2AI;
unsigned long gulA2DI;
unsigned long gulA2DI_prev;
double dtarget_drill_velocity;
double dtarget_bend_velocity;
double dtarget_bend_angle = -19023.5;

const int k_drill_speed_max_val = 10649600;
const float k_bend_angle_min = -37047;
const float k_bend_angle_max = -1000;

const double k_joint_knob_deadzone = 200;
const double k_drill_knob_deadzone = 200;

//burr
const int k_burr_speed_max_val = 20000; //rpm
const float k_burr_angle_min = -20480;
const float k_burr_angle_max = -200;

short gsA3AI;
short gsA4AI;

unsigned long gulA4DI;
unsigned long gulA4DI_prev;

double dtarget_burr_velocity;
double dtarget_burr_bend_velocity;
double dtarget_burr_angle = -2000;


//
/*
 ============================================================================
 Global structures for Elmo's Function Blocks
 ============================================================================
 */
MMC_CONNECT_HNDL gConnHndl;					// Connection Handle
CMMCConnection cConn;

//8mm drill
// a1 stands for joint angle motor
// a2 stands for drill spinning motor

//5.5mm drill
// a3 stands for joint angle motor
// a4 stands for drill spinning motor


// TODO : Update the names and number of the axes in the system
CMMCSingleAxis a1, a2, a3, a4;
CMMCHostComm cHost;
//MMC_MODBUSWRITEHOLDINGREGISTERSTABLE_IN 	mbus_write_in;
//MMC_MODBUSWRITEHOLDINGREGISTERSTABLE_OUT 	mbus_write_out;
MMC_MOTIONPARAMS_SINGLE stSingleDefault;	// Single axis default data


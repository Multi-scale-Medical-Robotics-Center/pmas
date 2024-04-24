/*
 ============================================================================
 Name : 	HandheldDrill.cpp
 Author :	Benjamin Spitzer
 Version :	1.00
 Description : The following example supports the following functionalities:

 The following features are demonstrated for a Ethercat network (Assuming PDOs were initalized via Ethercat configurator):

 - Two separate  state machines for handling parallel motions / sequences.
 - Modbus callback registration.
 - Emergency callback registration.
 - Empty modbus reading area.

 The program works with 2 axes - a01 and a02.

 ============================================================================
 */
#include "mmc_definitions.h"
#include "mmcpplib.h"
#include "UDP.h"
#include "HandheldDrill.h"		// Application header file.
#include <iostream>
#include <sys/time.h>			// For time structure
#include <signal.h>				// For Timer mechanism
/*
 ============================================================================
 Function:				main()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 The main function of this sample project.
 ============================================================================
 */

int main() {
	//
	//	Initialize system, axes and all needed initializations
	//
	MainInit();
	//
	//	Execute the state machine to handle the system sequences and control
	//
	MachineSequences();
	//
	//	Close what needs to be closed before program termination
	//
	MainClose();
	//
	return 1;// Terminate the application program back to the Operating System
}
/*
 ============================================================================
 Function:				MainInit()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Initilaize the system, including axes, communication, etc.
 ============================================================================
 */
void MainInit() {
//
//	Here will come initialization code for all system, axes, communication etc.
//
// 	InitializeCommunication to the GMAS:
//
	cout << "InitializeCommunication to the GMAS" << endl;
	gConnHndl = cConn.ConnectIPCEx(0x7fffffff, (MMC_MB_CLBK) CallbackFunc);
	//
	// Start the Modbus Server:
//	cHost.MbusStartServer(gConnHndl,1) ;
	//
	// Register Run Time Error Callback function
	CMMCPPGlobal::Instance()->RegisterRTE(OnRunTimeError);

	// Register the callback function for Modbus and Emergency:
//	cConn.RegisterEventCallback(MMCPP_MODBUS_WRITE,(void*)ModbusWrite_Received) ;
	cConn.RegisterEventCallback(MMCPP_EMCY, (void*) Emergency_Received);
//
// Initialize default parameters. This is not a must. Each parameter may be initialized individually.
	stSingleDefault.fEndVelocity = 0;
	stSingleDefault.dbDistance = 100000;
	stSingleDefault.dbPosition = 0;
	stSingleDefault.fVelocity = 100000;
	stSingleDefault.fAcceleration = 2000000;
	stSingleDefault.fDeceleration = 2000000;
	stSingleDefault.fJerk = 20000000;
	stSingleDefault.eDirection = MC_POSITIVE_DIRECTION;
	stSingleDefault.eBufferMode = MC_BUFFERED_MODE;
	stSingleDefault.ucExecute = 1;
	a1.InitAxisData("a01", gConnHndl);
	a1.SetDefaultParams(stSingleDefault);

	//
// 	TODO: Update number of necessary axes:
//
	stSingleDefault.fAcceleration = 50000000;
	stSingleDefault.fDeceleration = 50000000;
	stSingleDefault.fJerk = 20000000;
	a2.InitAxisData("a02", gConnHndl);
	a2.SetDefaultParams(stSingleDefault);


//
	stSingleDefault.fEndVelocity = 0;
	stSingleDefault.dbDistance = 100000;
	stSingleDefault.dbPosition = 0;
	stSingleDefault.fVelocity = 100000;
	stSingleDefault.fAcceleration = 2000000;
	stSingleDefault.fDeceleration = 2000000;
	stSingleDefault.fJerk = 20000000;
	stSingleDefault.eDirection = MC_POSITIVE_DIRECTION;
	stSingleDefault.eBufferMode = MC_BUFFERED_MODE;
	stSingleDefault.ucExecute = 1;
	a3.InitAxisData("a03", gConnHndl);
	a3.SetDefaultParams(stSingleDefault);

	//
	stSingleDefault.fAcceleration = 50000000;
	stSingleDefault.fDeceleration = 50000000;
	stSingleDefault.fJerk = 20000000;
	a4.InitAxisData("a04", gConnHndl);
	a4.SetDefaultParams(stSingleDefault);
	//
	// Set default motion parameters. TODO: Update for all axes.

	//
	// You may of course change internal parameters manually:
//	a1.m_fAcceleration=10000;
	giA1Status = a1.ReadStatus();
	if (giA1Status & NC_AXIS_ERROR_STOP_MASK) {
		a1.Reset();
		sleep(1);
		giA1Status = a1.ReadStatus();
		if (giA1Status & NC_AXIS_ERROR_STOP_MASK) {
			cout << "Axis a1 in Error Stop. Aborting.";
			exit(0);
		}
	}
	giA2Status = a2.ReadStatus();
	if (giA2Status & NC_AXIS_ERROR_STOP_MASK) {
		a2.Reset();
		sleep(1);
		giA2Status = a2.ReadStatus();
		if (giA2Status & NC_AXIS_ERROR_STOP_MASK) {
			cout << "Axis a2 in Error Stop. Aborting.";
			exit(0);
		}
	}

	giA3Status = a3.ReadStatus();
	if (giA3Status & NC_AXIS_ERROR_STOP_MASK) {
		a3.Reset();
		sleep(1);
		giA3Status = a3.ReadStatus();
		if (giA3Status & NC_AXIS_ERROR_STOP_MASK) {
			cout << "Axis a3 in Error Stop. Aborting.";
			exit(0);
		}
	}

	giA4Status = a4.ReadStatus();
	if (giA4Status & NC_AXIS_ERROR_STOP_MASK) {
		a4.Reset();
		sleep(1);
		giA4Status = a4.ReadStatus();
		if (giA4Status & NC_AXIS_ERROR_STOP_MASK) {
			cout << "Axis a4 in Error Stop. Aborting.";
			exit(0);
		}
	}


	//
	// Clear the modbus memory array:

//	memset(mbus_write_in.regArr,0x0,250) ;
	return;
}
/*
 ============================================================================
 Function:				MainClose()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Close all that needs to be closed before the application progra, is
 terminated.
 ============================================================================
 */
void MainClose() {
//
//	Here will come code for all closing processes
//
	cHost.MbusStopServer();
	MMC_CloseConnection(gConnHndl);
	return;
}
/*
 ============================================================================
 Function:				MachineSequences()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Starts the Main Timer function that will execute the states machines
 to control the system. Also performs a slow background loop for
 less time-critical background processes and monitoring of requests
 to terminate the application.
 ============================================================================
 */
void MachineSequences() {
//
//	Init all variables of the states machines
//
	MachineSequencesInit();
//
//	Enable MachineSequencesTimer() every TIMER_CYCLE ms
//
	EnableMachineSequencesTimer(TIMER_CYCLE_US);
//
//	Background loop. Handles termination request and other less time-critical background proceses
//
	while (!giTerminate) {
//		MachineSequencesTimer(0);
//
//		Execute background process if required
//
		BackgroundProcesses();
//
//		Sleep for ~SLEEP_TIME microseconds to reduce CPU load
//
		usleep(SLEEP_TIME);
	}
//
//	Termination requested. Close what needs to be closed at the states machines
//
	MachineSequencesClose();

	return;		// Back to the main() for program termination
}
/*
 ============================================================================
 Function:				MachineSequencesInit()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Initialize the states machines variables
 ============================================================================
 */
void MachineSequencesInit() {
//
//	Initializing all variables for the states machines
//
	giTerminate = FALSE;

	giStateA1Home = eIDLE;
	giPrevStateA1Home = eIDLE;
	giSubStateA1Home = eIDLE;
	//
	giStateRun = eIDLE;
	giPrevStateRun = eIDLE;
	giSubStateRun = eIDLE;

	giReentrance = FALSE;

	return;
}
/*
 ============================================================================
 Function:				MachineSequencesClose()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Close all that needs to be closed at the states machines before the
 application program is terminated.
 ============================================================================
 */
void MachineSequencesClose() {
//
//	Here will come code for all closing processes
//
	return;
}
/*
 ============================================================================
 Function:				BackgroundProcesses()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Performs background processes that are not managed by the States Machines.
 This can be saving logs to the FLASH, performing background calculations etc.
 Generally speaking, this function, although colored in red in the Close all that needs to be closed at the states machines before the
 application program is terminated.
 ============================================================================
 */
void BackgroundProcesses() {
//
//	Here will come code for all closing processes
//
//
	UDPDemoServer();

	// Debug usage
	if ((gulA2DI_prev == 0 and gulA2DI == 1) or (gulA4DI_prev == 0 and gulA4DI == 1)) {
		// every time user press the foot pedal print out debug message
		cout << "dtarget_bend_angle: " << dtarget_bend_angle
				<< "   dtarget_drill_velocity: " << dtarget_drill_velocity
				<< endl
				<<  "dtarget_burr_velocity: " <<dtarget_burr_velocity
				<< "   dtarget_burr_angle: " <<dtarget_burr_angle
				<< endl;
	}

	static int bgp_counter = 0;
	if (bgp_counter++ == 5000)
	{
		cout << "State monitor (10s) - MachineSeq giReentrance_counts: " << giReentrance_counts << endl;
//		cout << "\t\t - encoder counts: " << gfA1Position << endl;
		bgp_counter = 0;
		giReentrance_counts = 0;

		// print current motor status
//		cout << "gfA1Torque: " << gfA1Torque << endl;
//		cout << "gfA2Torque: " << gfA2Torque << endl;
//		cout << "gfA1Position: " << gfA1Position << endl;
//		cout << "gfA2Velocity: " << gfA2Velocity << endl;
	}

	return;
}
/*
 ============================================================================
 Function:				EnableMachineSequencesTimer()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Enables the main machine sequences timer function, to be executed each
 TIMER_CYCLE us.
 ============================================================================
 */
void EnableMachineSequencesTimer(int TimerCycle_us) {
	struct itimerval timer;
	struct sigaction stSigAction;

	// Whenever a signal is caught, call TerminateApplication function
	stSigAction.sa_handler = TerminateApplication;

	sigaction(SIGINT, &stSigAction, NULL);
	sigaction(SIGTERM, &stSigAction, NULL);
	sigaction(SIGABRT, &stSigAction, NULL);
	sigaction(SIGQUIT, &stSigAction, NULL);
//
//	Enable the main machine sequences timer function
//
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = TimerCycle_us;	// From ms to micro seconds
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = TimerCycle_us;		// From ms to micro seconds

	setitimer(ITIMER_REAL, &timer, NULL);		//- Temporarily !!!

	signal(SIGALRM, MachineSequencesTimer); // every TIMER_CYCLE ms SIGALRM is received which calls MachineSequencesTimer()

	return;
}
/*
 ============================================================================
 Function:				MachineSequencesTimer()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 A timer function that is called by the OS every TIMER_CYCLE ms.
 It executes the machine sequences states machines and actully controls
 the sequences and behavior of the machine.
 ============================================================================
 */
void MachineSequencesTimer(int iSig) {
//
//	In case the application is waiting for termination, do nothing.
//	This can happen if giTerminate has been set, but the background loop
//	didn't handle it yet (it has a long sleep every loop)
//
	if (giTerminate == TRUE)
		return;
//
//	Avoid reentrance of this time function
//
//	Reentrance can theoretically happen if the execution of this timer function
//	is wrongly longer than TIMER_CYCLE. In such case, reentrance should be avoided
//	to prevent unexpected behavior (the code is not designed for reentrance).
//
//	In addition, some error handling should be taken by the user.
//
	if (giReentrance) {
//
//		Print an error message and return. Actual code should take application related error handling
		giReentrance_counts ++;
//		printf("MachineSequencesTimer Re-entrancy!\n");
		return;
	}

	giReentrance = TRUE; // to enable detection of re-entrancy. The flag is cleared at the end of this function
//
//	Read all input data.
//
//	Here, every TIMER_CYCLE ms, the user should read all input data that may be
//	required for the states machine code and copy them into "mirror" variables.
//
//	The states machines code, below, should use only the mirror variables, to ensure
//	that all input data is synchronized to the timer event.
//
//	Input data can be from the Host (MODBUS) or from the drives or I/Os units
//	(readingfrom the GMAS core firmware using one of the Function Blocks library
//	functions) or from any other source.
//
	ReadAllInputData();
	/*
	 ============================================================================

	 States Machines code starts here!

	 ============================================================================
	 */

//
//	In case it is a new state value, clear also the value of the sub-state
//	to ensure it will start from its beginning (from the first ssub-state)
//
	if (giState1 != giPrevState1) {
		giSubState1 = FIRST_SUB_STATE;
		giPrevState1 = giState1;
	}
//
//	if (giState2 != giPrevState2)
//	{
//		giSubState2 	= FIRST_SUB_STATE;
//		giPrevState2 	= giState2;
//	}
//	Handle the main state machine.
//
//	The value of the State variable is used to make decisions of the main states machine and to call,
//	as y, the relevant function that handles to process itslef in a sub-state machine.
//
	switch (giState1) {
//
//		Do nothing, waiting for commands
//
	case eIDLE: {
		giState1 = eA1Home;
		break;
	}
//
//		Do State Machine1
//
	case eA1Home: {
		StateFunction_eA1Home(a1,giA1Status); // calls a sub-state machine function to handle this proocess
		break;
	}

	case eA2Start: {
		StateFunction_eA2Start(a2,giA2Status);
		break;
	}

	case eA3Home: {
		StateFunction_eA1Home(a3,giA3Status); // calls a sub-state machine function to handle this proocess
		break;
	}

	case eA4Start: {
		StateFunction_eA2Start(a4,giA4Status);
		break;
	}

//		Do State Machine2

	case eRun: {
		StateFunction_eRun(); // calls a sub-state machine function to handle this proocess
		break;
	}

	case eError: {
		break;
	}
//
//		The default case. Should not happen, the user can implement error handling.
//
	default: {
		break;
	}
	}

//
//	Write all output data
//
//	Here, every TIMER_CYCLE ms, after the execution of all states machines
//	(based on all the Input Data as read from all sources at teh top of this function)
//	the user should write all output data (written into mirror variables within the
//	states machines code) to the "external world" (MODBUS, GMAS FW core, ...).
//
//	After alll states machines were executed and generated their outputs, the outputs
//	are writen to the "external world" to actually execute the states machines "decisions"
//
	WriteAllOutputData();
//
//	Clear the reentrancy flag. Now next execution of this function is allowed
//
	giReentrance = FALSE;
//
	return;// End of the sequences timer function. The function will be triggered again upon the next timer event.
}
/*
 ============================================================================
 Function:				ReadAllInputData()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 The data used during the processing of the states machines must be synchronized with the
 timer event and must be "frozen" during the processing of all states machines code, so that
 all decisions and calculations will use the same data.

 This function is called at the beginning of the timer function, each TIMER_CYCLE ms,
 to collect all required input data (as relevant to each application) and to copy
 it into "mirror" variables, that will be used by the state machines code.
 ============================================================================
 */
void ReadAllInputData() {
	giA1Status = a1.ReadStatus();
	giA2Status = a2.ReadStatus();
	giA3Status = a3.ReadStatus();
	giA4Status = a4.ReadStatus();



	gfA1Torque = a1.GetActualTorque();
	gfA2Torque = a2.GetActualTorque();
	gfA1Position = a1.GetActualPosition();
	gfA2Velocity = a2.GetActualVelocity();

//	// debug
//	cout << "gfA1Torque: " << gfA1Torque << endl;
//	cout << "gfA2Torque: " << gfA2Torque << endl;
//	cout << "gfA1Position: " << gfA1Position << endl;
//	cout << "gfA2Velocity: " << gfA2Velocity << endl;



	int rc;

	MMC_READPIVARSHORT_IN InParam2;
	InParam2.usIndex = 7;		//index of process image
	InParam2.ucDirection = ePI_INPUT;

	MMC_READPIVARSHORT_OUT OutParam2;
	// Read AI from A1
	rc = MMC_ReadPIVarShort(gConnHndl, a1.GetRef(), &InParam2, &OutParam2);
	gsA1AI = OutParam2.sData;
//	cout << "AD1: " << OutParam2.sData << endl;
	// Read AI from A2
	rc = MMC_ReadPIVarShort(gConnHndl, a2.GetRef(), &InParam2, &OutParam2);
	gsA2AI = OutParam2.sData;
//	cout << "AD2: " << OutParam2.sData << endl;

	rc = MMC_ReadPIVarShort(gConnHndl, a3.GetRef(), &InParam2, &OutParam2);
	gsA3AI = OutParam2.sData;
//	cout << "AD1: " << OutParam2.sData << endl;
	// Read AI from A2
	rc = MMC_ReadPIVarShort(gConnHndl, a4.GetRef(), &InParam2, &OutParam2);
	gsA4AI = OutParam2.sData;
//	cout << "AD2: " << OutParam2.sData << endl;

	gulA2DI_prev = gulA2DI; // get the prev foot pedal state
	gulA2DI = !(a2.GetDigInputs() & 0x040000);
//	cout << "DI2: " << gulA2DI << endl;

	gulA4DI_prev = gulA4DI; // get the prev foot pedal state
	gulA4DI = !(a4.GetDigInputs() & 0x040000);
//	cout << "DI2: " << gulA2DI << endl;


	// update socket data for next send
	sock_msg_elmo2pc.drill_torque_m = gfA2Torque;
	sock_msg_elmo2pc.drill_speed_m = gfA2Velocity;
	sock_msg_elmo2pc.drill_potentiometer_val = gsA2AI;
	sock_msg_elmo2pc.joint_angle_m = gfA1Position;
	sock_msg_elmo2pc.joint_torque_m = gfA1Torque;
	sock_msg_elmo2pc.foot_pedal = gulA2DI;
	sock_msg_elmo2pc.joint_potentiometer_val = gsA1AI;

	return;
}
/*
 ============================================================================
 Function:				WriteAllOutputData()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Write all the data, generated by the states machines code, to the "external
 world".

 This is done here (and not inside the states machines code) to ensure that
 all data is updated simultaneously and in a synchronous way.

 The states machines code write the data into "mirror" variables, that are here
 copied or sent to the "external world" (Host via MODBUS, GMAS core firmware, etc.)
 ============================================================================
 */
void WriteAllOutputData() {
	//
	//	Here should come the code to write/send all ouput data
	//
	if (giState1 == eRun && giSubStateRun == eSubState_SM_OK) {
		a1.MoveAbsolute(dtarget_bend_angle, MC_ABORTING_MODE);
		a2.MoveVelocity(dtarget_drill_velocity, MC_ABORTING_MODE);

		a3.MoveAbsolute(dtarget_burr_angle, MC_ABORTING_MODE);
		a4.MoveVelocity(dtarget_burr_velocity*4096/60, MC_ABORTING_MODE); //rpm to encoder lines;
	}
	return;
}
/*
 ============================================================================
 Function:				StateFunction_1()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 A sub-states machine function. This function executes the sub-states machine
 of the Function1 process.

 For instance, a homing state machine will consist of:

 Change Operation Mode.
 Power Enable
 Start homing - method number n
 Wait for end of homing

 Each step is handled by a dedicated function. However, calling a function
 is not a must and the relevant code for each sub-state can be directly
 written within the switch-case structure.
 ============================================================================
 */
void StateFunction_eA1Home(CMMCSingleAxis axis, int giStatus) {
//
//	Handle the sub-state machine.
//
//	The value of the Sub-State variable is used to make decisions of the sub-states machine and to call,
//	as necessary, the relevant function that handles to process itself.
//
	switch (giStateA1Home) {
//
//		Perform sub SM 1
//
	case eSubState_SM_A1Home_PowerOff: {
		//
		if (giState1 == eA1Home)
			cout<< "A1..........." << endl;
		else if (giState1 == eA3Home)
			cout<< "A3..........." << endl;

		if (giStatus & NC_AXIS_ERROR_STOP_MASK) {
			cout << "Axis in Error Stop. Aborting." << endl;
			exit(0);
		}
//			if (giStatus & NC_AXIS_STAND_STILL_MASK )
		{
			axis.PowerOff();
			cout << "Power Off Axis." << endl;
			giStateA1Home = eSubState_SM_A1Home_WaitPowerOff;
		}

		break;
	}
//
//		Perform sub SM 2
//
	case eSubState_SM_A1Home_WaitPowerOff: {
		if (giStatus & NC_AXIS_DISABLED_MASK) {
			cout << "Axis Powered Off." << endl;
			giStateA1Home = eSubState_SM_A1Home_SetHomeMode;
		}
		break;
	}
//
//		Perform sub SM 3
//
	case eSubState_SM_A1Home_SetHomeMode: {
		axis.SetOpMode(OPM402_HOMING_MODE);
		cout << "Axis set Homing mode." << endl;
		giStateA1Home = eSubState_SM_A1Home_WaitHomeMode;
		break;
	}

	case eSubState_SM_A1Home_WaitHomeMode: {
		if (axis.GetOpMode() == OPM402_HOMING_MODE)
		{
			cout << "Axis Wait Home Mode." << endl;
			giStateA1Home = eSubState_SM_A1Home_PowerOn;
		}
		break;
	}

	case eSubState_SM_A1Home_PowerOn: {
		axis.PowerOn();
		cout << "Axis Power On." << endl;
		giStateA1Home = eSubState_SM_A1Home_WaitPowerOn;
		break;
	}

	case eSubState_SM_A1Home_WaitPowerOn: {
		if ((!(giStatus & NC_AXIS_DISABLED_MASK))
				&& (giStatus & NC_AXIS_STAND_STILL_MASK)) {
			cout << "Axis Powered On." << endl;
			giStateA1Home = eSubState_SM_A1Home_StartHoming;
		}
		break;
	}

	case eSubState_SM_A1Home_StartHoming: {
		MMC_HOMEDS402EX_IN stDS402EXHome;

		if (giState1 == eA1Home){
			stDS402EXHome.dbPosition = 0;
			stDS402EXHome.dbDetectionVelocityLimit = 500;
			stDS402EXHome.fAcceleration = 100000;
			stDS402EXHome.fVelocityHi = 5000;
			stDS402EXHome.fVelocityLo = 5000;
			stDS402EXHome.fDistanceLimit = 50000;
			stDS402EXHome.fTorqueLimit = 0.3;
			stDS402EXHome.uiHomingMethod = -2;
			stDS402EXHome.uiTimeLimit = 10000;
			stDS402EXHome.uiDetectionTimeLimit = 200;
			stDS402EXHome.ucExecute = 1;
			stDS402EXHome.eBufferMode = MC_BUFFERED_MODE;
		}
		else if (giState1 == eA3Home){
			stDS402EXHome.dbPosition = 0;
			stDS402EXHome.dbDetectionVelocityLimit = 500;
			stDS402EXHome.fAcceleration = 100000;
			stDS402EXHome.fVelocityHi = 5000;
			stDS402EXHome.fVelocityLo = 5000;
			stDS402EXHome.fDistanceLimit = 50000;
			stDS402EXHome.fTorqueLimit = 1.2;
			stDS402EXHome.uiHomingMethod = -2;
			stDS402EXHome.uiTimeLimit = 10000;
			stDS402EXHome.uiDetectionTimeLimit = 100;
			stDS402EXHome.ucExecute = 1;
			stDS402EXHome.eBufferMode = MC_BUFFERED_MODE;
		}

		axis.SetDefaultHomeDS402ExParams(stDS402EXHome);
		axis.HomeDS402Ex();
		giStateA1Home = eSubState_SM_A1Home_WaitHomingEnd;
		cout << "Axis Start Homing." << endl;
		break;
	}

	case eSubState_SM_A1Home_WaitHomingEnd: {
		if (giStatus & NC_AXIS_STAND_STILL_MASK) {
			cout << "Axis Finished Homing." << endl;
			giStateA1Home = eSubState_SM_A1Home_StartProfilePositionMode;
		}
		break;
	}

	case eSubState_SM_A1Home_StartProfilePositionMode: {
		cout << "Axis Start Profile Position Mode." << endl;
		axis.SetOpMode(OPM402_PROFILE_POSITION_MODE);
		giStateA1Home = eSubState_SM_A1Home_WaitProfilePositionMode;
		break;
	}

	case eSubState_SM_A1Home_WaitProfilePositionMode: {
		if (axis.GetOpMode() == OPM402_PROFILE_POSITION_MODE) {
			cout << "Axis Profile Position Mode." << endl;
			giStateA1Home = eSubState_SM_A1Home_GoZero;
		}
		break;
	}

	case eSubState_SM_A1Home_GoZero: {
		cout << "Axis Start Going to Zero." << endl;
//			static int cnt = 0;
//
//			if (cnt > -40000)
//			{
//				int tar_pos = cnt;
//				axis.MoveAbsolute(tar_pos, MC_ABORTING_MODE);
//				cnt -= 10;
//			}
//			else
		{
			//-36047, -18481, 0
			if (giState1 == eA1Home)
				axis.MoveAbsolute(dtarget_bend_angle, MC_ABORTING_MODE);
			else if (giState1 == eA3Home)
				axis.MoveAbsolute(dtarget_burr_angle, MC_ABORTING_MODE);

			giStateA1Home = eSubState_SM_A1Home_Done;
		}

		break;
	}
	case eSubState_SM_A1Home_Done: {
		if (giStatus & NC_AXIS_STAND_STILL_MASK) {
			cout << "Axis at Zero." << endl << endl;
			if (giState1 == eA1Home){
				giState1 = eA2Start;
				giStateA1Home = eSubState_SM_A1Home_PowerOff;
			}
			else if (giState1 == eA3Home)
				giState1 = eA4Start;

		}
		break;
	}
//
//		Perform sub SM 4
//
	case eSubState_SM_A1Home_Error: {
		break;
	}
//
//		The default case. Should not happen, the user can implement error handling.
//
	default: {
		break;
	}
	}
//
	return;
}

void StateFunction_eA2Start(CMMCSingleAxis axis, int giStatus) {

	switch (giStateA2Start) {
	case eSubState_SM_A2Start_PowerOff: {
		if (giState1 == eA2Start)
			cout<< "A2..........." << endl;
		else if (giState1 == eA4Start)
			cout<< "A4..........." << endl;

		if (giStatus & NC_AXIS_ERROR_STOP_MASK) {
			cout << "Axis in Error Stop. Aborting." << endl;
			exit(0);
		}
//			if (giStatus & NC_AXIS_STAND_STILL_MASK )
		{
			axis.PowerOff();
			cout << "Power Off Axis." << endl;
			giStateA2Start = eSubState_SM_A2Start_WaitPowerOff;
		}
		break;
	}
	case eSubState_SM_A2Start_WaitPowerOff: {
		if (giStatus & NC_AXIS_DISABLED_MASK) {
			cout << "Axis Powered Off." << endl;
			giStateA2Start = eSubState_SM_A2Start_SetVelocityMode;
		}
		break;
	}
	case eSubState_SM_A2Start_SetVelocityMode: {
		axis.SetOpMode(OPM402_PROFILE_VELOCITY_MODE);
		cout << "Axis set Velocity mode." << endl;
		giStateA2Start = eSubState_SM_A2Start_WaitVelocityMode;
		break;
	}
	case eSubState_SM_A2Start_WaitVelocityMode: {
		if (axis.GetOpMode() == OPM402_PROFILE_VELOCITY_MODE) {
			cout << "Axis Wait Velocity Mode." << endl;
			giStateA2Start = eSubState_SM_A2Start_PowerOn;
		}
		break;
	}
	case eSubState_SM_A2Start_PowerOn: {
		axis.PowerOn();
		cout << "Axis Power On." << endl;
		giStateA2Start = eSubState_SM_A2Start_WaitPowerOn;
		break;
	}
	case eSubState_SM_A2Start_WaitPowerOn: {
		if ((!(giStatus & NC_AXIS_DISABLED_MASK))
				&& (giStatus & NC_AXIS_STAND_STILL_MASK)) {
			cout << "Axis Powered On." << endl;
			giStateA2Start = eSubState_SM_A2Star_Done;
		}
		break;
	}
	case eSubState_SM_A2Star_Done: {
		if (giStatus & NC_AXIS_STAND_STILL_MASK) {
			cout << "Axis Start Done." << endl << endl;

			if (giState1 == eA2Start){
				giState1 = eA3Home;
				giStateA2Start = eSubState_SM_A2Start_PowerOff;
			}
			else if (giState1 == eA4Start)
				giState1 = eRun;
		}
		break;
	}
	case eSubState_SM_A2Start_Error:
//
//		The default case. Should not happen, the user can implement error handling.
//
	default: {
		break;
	}
	}
//
	return;
}



double dead_zone(double in, double dead_zone) {
	double rtn = 0;
	if (in > dead_zone) {
		rtn = in - dead_zone;
	} else if (in < -dead_zone) {
		rtn = in + dead_zone;
	}

	return rtn;
}
double limit(double in, double low, double up) {
	double rtn = in;
	if (in > up) {
		rtn = up;
	} else if (in < low) {
		rtn = low;
	}

	return rtn;
}
void StateFunction_eRun() {
	switch (giStateRun) {

	case eSubState_SM_OK: {
		// 1 check if taken over by pc udp client
		if (is_udp_cmd_valid and sock_msg_pc2elmo.pc_take_overs) {
			// control taking over by pc
			static float _d_drill_vel = 0.0;
			static float _d_joint_pos = 0.0;

			if (sock_msg_pc2elmo.spin_on) {
				_d_drill_vel = sock_msg_pc2elmo.drill_speed_scaled;
			} else {
				_d_drill_vel = 0;
			}

			_d_joint_pos = sock_msg_pc2elmo.joint_target_counts;

			// limit the scaled val to [-1, 1]
			_d_drill_vel = limit(_d_drill_vel, -1.0, 1.0);
			_d_joint_pos = limit(_d_joint_pos, k_bend_angle_min, k_bend_angle_max);

			// scale them
			dtarget_drill_velocity = _d_drill_vel * k_drill_speed_max_val;
			dtarget_bend_angle = _d_joint_pos;
		}

		// 2 udp cmd invalid, or pc choose not to take over control
		if (not is_udp_cmd_valid or not sock_msg_pc2elmo.pc_take_overs) {

			// Independent control
 			if (gulA2DI) {
 				//dtarget_drill_velocity = 100000;
 				dtarget_drill_velocity = dead_zone(gsA2AI, k_drill_knob_deadzone) / 5000 * k_drill_speed_max_val;
 			} else {
 				dtarget_drill_velocity = 0;
 			}


			// TODO: disable the drill velocity control by the knob reading
//			dtarget_drill_velocity = 0;

			//center: 1640 +- 20, max: 4930, min: 220
			dtarget_bend_velocity = dead_zone(gsA1AI - 1640, k_joint_knob_deadzone);

			if (dtarget_bend_velocity > 0.001) {
				dtarget_bend_velocity = 300 * dtarget_bend_velocity
						/ (4930 - 1640);
			} else if (dtarget_bend_velocity < -0.001) {
				dtarget_bend_velocity = 300 * dtarget_bend_velocity
						/ (1640 - 220);
			}
			dtarget_bend_angle += dtarget_bend_velocity;
		}


		//burr control
		if (gulA4DI) {
			//dtarget_burr_velocity = 10000; //rpm
			dtarget_burr_velocity = dead_zone(5200-gsA4AI, k_drill_knob_deadzone) / 3000 * k_burr_speed_max_val + 1000;
		} else {
			dtarget_burr_velocity = 0;
		}


		dtarget_burr_bend_velocity = dead_zone(gsA3AI - 1420, k_joint_knob_deadzone);
		if (dtarget_burr_bend_velocity > 0.005) {
			dtarget_burr_bend_velocity = 100 * dtarget_burr_bend_velocity
					/ (4930 - 1420);
		} else if (dtarget_burr_bend_velocity < -0.005) {
			dtarget_burr_bend_velocity = 100 * dtarget_burr_bend_velocity
					/ (1420 - 100);
		}
		dtarget_burr_angle += dtarget_burr_bend_velocity;


		// 3  limit output
		dtarget_drill_velocity = limit(dtarget_drill_velocity,
				-k_drill_speed_max_val, k_drill_speed_max_val);

		dtarget_bend_angle = limit(dtarget_bend_angle,
				k_bend_angle_min, k_bend_angle_max);

		dtarget_burr_velocity = limit(dtarget_burr_velocity,
				-k_burr_speed_max_val, k_burr_speed_max_val);

		dtarget_burr_angle = limit(dtarget_burr_angle,
				k_burr_angle_min, k_burr_angle_max);

		break;
	}

	case eSubState_SM_Error: {
		break;
	}
	default: {
		break;
	}
	}
	return;
}

/*
 ============================================================================
 Function:				SubState2_1Function()
 Input arguments:		None.
 Output arguments: 		None.
 Returned value:		None.
 Version:				Version 1.00
 Updated:				10/03/2011
 Modifications:			N/A

 Description:

 Starts an XY motion as part of the states machine code for X,Y move,
 and chage the sub state to wait for the end of motions.
 ============================================================================
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Function name	:	void callback function																		//
//	Created			:	Version 1.00																				//
//	Updated			:	3/12/2010																					//
//	Modifications	:	N/A																							//
//	Purpose			:	interupt function 																			//
//																													//
//	Input			:	N/A																							//
//	Output			:	N/A																							//
//	Return Value	:	int																							//
//	Modifications:	:	N/A																							//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CallbackFunc(unsigned char* recvBuffer, short recvBufferSize,
		void* lpsock) {
	// Which function ID was received ...
	switch (recvBuffer[1]) {
	case EMCY_EVT:
		//
		// Please note - The emergency event was registered.
		// printf("Emergency Event received\r\n") ;
		break;
	case MOTIONENDED_EVT:
		printf("Motion Ended Event received\r\n");
		break;
	case HBEAT_EVT:
		printf("H Beat Fail Event received\r\n");
		break;
	case PDORCV_EVT:
		printf("PDO Received Event received - Updating Inputs\r\n");
		break;
	case DRVERROR_EVT:
		printf("Drive Error Received Event received\r\n");
		break;
	case HOME_ENDED_EVT:
		printf("Home Ended Event received\r\n");
		break;
	case SYSTEMERROR_EVT:
		printf("System Error Event received\r\n");
		break;
		/* This is commented as a specific event was written for this function. Once it occurs
		 * the ModbusWrite_Received will be called
		 case MODBUS_WRITE_EVT:
		 // TODO Update additional data to be read such as function parameters.
		 // TODO Remove return 0 if you want to handle as part of callback.
		 return 0;
		 printf("Modbus Write Event received - Updating Outputs\r\n") ;

		 break ;
		 */
	}
	//
	return 1;
}

int OnRunTimeError(const char *msg, unsigned int uiConnHndl,
		unsigned short usAxisRef, short sErrorID, unsigned short usStatus) {
	MMC_CloseConnection(uiConnHndl);
	printf(
			"MMCPPExitClbk: Run time Error in function %s, axis ref=%d, err=%d, status=%d, bye\n",
			msg, usAxisRef, sErrorID, usStatus);
	exit(0);
}

///////////////////////////////////////////////////////////////////////
//	Function name	:	void terminate_application(int iSigNum)
//	Created			:	Version 1.00
//	Updated			:	20/05/2010
//	Modifications	:	N/A
//	Purpose			:	Called in case application is terminated, stop modbus, engines, and power off engines
//	Input			:	int iSigNum - Signal Num.
//	Output			:	N/A
//	Return Value	:	void
//
//	Modifications:	:	N/A
//////////////////////////////////////////////////////////////////////
void TerminateApplication(int iSigNum) {
	//
	printf("In Terminate Application ...\n");
	giTerminate = 1;
	sigignore(SIGALRM);
	//
	// Wait till other threads exit properly.
	sleep(1);
	return;
}

//
// Callback Function once a Modbus message is received.
void ModbusWrite_Received() {
	printf("Modbus Write Received\n");
}
//
// Callback Function once an Emergency is received.
void Emergency_Received(unsigned short usAxisRef, short sEmcyCode) {
	printf("Emergency Message Received on Axis %d. Code: %x\n", usAxisRef,
			sEmcyCode);
}

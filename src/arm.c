/*-----------------------------------------------------------------------------*/
/** @file    arm.c                                                             */
/** @brief   The arm system for the robot                                      */
/*-----------------------------------------------------------------------------*/

#include "arm.h"

#include <math.h>
#include <stdlib.h>

// storage for arm
static arm_t arm;

// working area for arm task
static WORKING_AREA(waArm, 512);

// private functions
static msg_t	armThread(void *arg);
static void		armPIDUpdate(int16_t *cmd);

// arm speed adjustment
#define USE_ARM_SPEED_TABLE 1
#ifdef USE_ARM_SPEED_TABLE

const unsigned int armSpeedTable[128] =
{
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0, 21, 21, 21, 22, 22, 22, 23, 24, 24,
	 25, 25, 25, 25, 26, 27, 27, 28, 28, 28,
	 28, 29, 30, 30, 30, 31, 31, 32, 32, 32,
	 33, 33, 34, 34, 35, 35, 35, 36, 36, 37,
	 37, 37, 37, 38, 38, 39, 39, 39, 40, 40,
	 41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
	 46, 47, 47, 48, 48, 49, 50, 50, 51, 52,
	 52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
	 61, 62, 63, 64, 65, 66, 67, 67, 68, 70,
	 71, 72, 72, 73, 74, 76, 77, 78, 79, 79,
	 80, 81, 83, 84, 84, 86, 86, 87, 87, 88,
	 88, 89, 89, 90, 90,127,127,127
};

static inline int
armSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (((speed > 0) - (speed < 0)) * armSpeedTable[abs(speed)]);
}

#else

static inline int
armSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (speed);
}

#endif

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to arm structure - not used locally                */
/** @return     A arm_t pointer                                                */
/*-----------------------------------------------------------------------------*/
arm_t *
armGetPtr(void)
{
	return (&arm);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign motor and potentiometer to the arm system.              */
/** @param[in]  topMotorPair The arm top motor pair                            */
/** @param[in]  middleMotorPair The arm middle motor pair                      */
/** @param[in]  bottomMotorPair The arm bottom motor pair                      */
/** @param[in]  potentiometer The arm potentiometer                            */
/** @param[in]  reversed Is the arm potentiometer reversed?                    */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  downValue The arm potentiometer down value                     */
/** @param[in]  bumpValue The arm potentiometer bump value                     */
/** @param[in]  upValue The arm potentiometer up value                         */
/*-----------------------------------------------------------------------------*/
void
armSetup(tVexMotor motor0, tVexMotor motor1, tVexMotor motor2,
		 tVexAnalogPin potentiometer, bool_t reversed, float gearRatio,
		 int16_t downValue, int16_t bumpValue, int16_t upValue)
{
	arm.motor0 = motor0;
	arm.motor1 = motor1;
	arm.motor2 = motor2;
	arm.potentiometer = potentiometer;
	arm.reversed = reversed;
	arm.gearRatio = gearRatio;
	arm.downValue = downValue;
	arm.bumpValue = bumpValue;
	arm.upValue = upValue;
	arm.lock = NULL;
	arm.isDown = FALSE;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the arm system.                                     */
/*-----------------------------------------------------------------------------*/
void
armInit(void)
{
	// SmartMotorSetRpmSensor(arm.topMotorPair, arm.potentiometer, 6000 * arm.gearRatio, arm.reversed);
	// SmartMotorSetRpmSensor(arm.middleMotorPair, arm.potentiometer, 6000 * arm.gearRatio, arm.reversed);
	// SmartMotorSetRpmSensor(arm.bottomMotorPair, arm.potentiometer, 6000 * arm.gearRatio, arm.reversed);
	SmartMotorLinkMotors(arm.motor2, arm.motor1);
	SmartMotorLinkMotors(arm.motor2, arm.motor0);
	arm.lock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	arm.lock->enabled = 0;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the arm system thread                                    */
/*-----------------------------------------------------------------------------*/
void
armStart(void)
{
	chThdCreateStatic(waArm, sizeof(waArm), NORMALPRIO - 1, armThread, NULL);
	return;
}

#define IMMEDIATE_TIMEOUT 1000

/*-----------------------------------------------------------------------------*/
/** @brief      The arm system thread                                          */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
armThread(void *arg)
{
	int16_t armCmd = 0;
	bool_t immediate = FALSE;
	int16_t immediateTimeout = 0;
	bool_t isDown = FALSE;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("arm");

	while (!chThdShouldTerminate()) {
		armCmd = armSpeed( vexControllerGet( Ch2Xmtr2 ) );
		// armCmd = armSpeed( vexControllerGet( Ch3Xmtr2 ) );
		// armCmd = armSpeed( vexControllerGet( Ch3Xmtr2 ) );

		// (void) armPIDUpdate;

		immediate = TRUE;

		// if (immediateTimeout < 0) {
		// 	immediateTimeout = 0;
		// }

		// if (armCmd == 0) {
		// 	if (immediateTimeout > 0) {
		// 		immediateTimeout -= 25;
		// 	}
		// 	immediate = FALSE;
		// 	if (vexControllerGet( Btn7D ) || vexControllerGet( Btn7DXmtr2 )) {
		// 		arm.isDown = TRUE;
		// 		arm.lock->enabled = 1;
		// 		arm.lock->target_value = arm.downValue;
		// 	} else if (vexControllerGet( Btn7U ) || vexControllerGet( Btn7UXmtr2 )) {
		// 		arm.isDown = FALSE;
		// 		arm.lock->enabled = 1;
		// 		arm.lock->target_value = arm.upValue;
		// 	} else if (vexControllerGet( Btn7L ) || vexControllerGet( Btn7LXmtr2 )) {
		// 		arm.isDown = TRUE;
		// 		arm.lock->enabled = 1;
		// 		arm.lock->target_value = arm.bumpValue;
		// 	}
		// 	armPIDUpdate(&armCmd);
		// 	// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "error %f", arm.lock->error);
		// } else {
		// 	arm.isDown = FALSE;
		// 	immediate = FALSE;
		// 	// disable PID if driving
		// 	arm.lock->enabled = 0;
		// 	PidControllerUpdate( arm.lock ); // zero out PID
		// 	if (isDown == TRUE && armCmd > 0) {
		// 		isDown = FALSE;
		// 		immediateTimeout = 0;
		// 	} else if (isDown == FALSE && armCmd < 0) {
		// 		isDown = TRUE;
		// 		immediateTimeout = 0;
		// 	}
		// 	if (immediateTimeout >= IMMEDIATE_TIMEOUT) {
		// 		immediate = FALSE;
		// 	} else {
		// 		immediate = TRUE;
		// 		immediateTimeout += 25;
		// 	}
		// }



		// SetMotor( arm.motor0, armCmd, FALSE );
		// SetMotor( arm.motor1, armCmd, FALSE );
		// SetMotor( arm.motor2, armCmd, FALSE );

		SetMotor( arm.motor0, armCmd, immediate );
		SetMotor( arm.motor1, armCmd, immediate );
		SetMotor( arm.motor2, armCmd, immediate );

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
armPIDUpdate(int16_t *cmd)
{
	// enable PID if not driving and already disabled
	if (arm.lock->enabled == 0) {
		arm.lock->enabled = 1;
		arm.lock->target_value = vexAdcGet( arm.potentiometer );
	}
	// prevent PID from trying to lock outside bounds
	if (arm.lock->target_value < arm.downValue)
		arm.lock->target_value = arm.downValue;
	else if (arm.lock->target_value > arm.upValue)
		arm.lock->target_value = arm.upValue;
	// update PID
	arm.lock->sensor_value = vexAdcGet( arm.potentiometer );
	arm.lock->error =
		(arm.reversed)
		? (arm.lock->sensor_value - arm.lock->target_value)
		: (arm.lock->target_value - arm.lock->sensor_value);
	*cmd = PidControllerUpdate( arm.lock );
	// limit output if error is small
	// if (fabs(arm.lock->error) < 100) {
	// 	*cmd = *cmd / 10;
	// } else if (fabs(arm.lock->error) < 300) {
	// 	*cmd = *cmd / 5;
	// } else
	if (arm.isDown && fabs(arm.lock->error) > 50) {
		*cmd = *cmd * 100;
	}
	// if (arm.isDown) {
	// 	*cmd = *cmd / 1;
	// // } else if (fabs(arm.lock->error) < 700) {
	// // 	*cmd = *cmd / 2;
	// }
	*cmd = armSpeed( *cmd );
	return;
}

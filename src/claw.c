/*-----------------------------------------------------------------------------*/
/** @file    claw.c                                                            */
/** @brief   The claw system for the robot                                     */
/*-----------------------------------------------------------------------------*/

#include "claw.h"

#include <math.h>
#include <stdlib.h>

// storage for claw
static claw_t claw;

// working area for claw task
static WORKING_AREA(waClaw, 512);

// private functions
static msg_t	clawThread(void *arg);
static void		clawPIDUpdate(int16_t *cmd);

// claw speed adjustment
#define USE_CLAW_SPEED_TABLE 1
#ifdef USE_CLAW_SPEED_TABLE

const unsigned int clawSpeedTable[128] =
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
clawSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (((speed > 0) - (speed < 0)) * clawSpeedTable[abs(speed)]);
}

#else

static inline int
clawSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (speed);
}

#endif

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to claw structure - not used locally               */
/** @return     A claw_t pointer                                               */
/*-----------------------------------------------------------------------------*/
claw_t *
clawGetPtr(void)
{
	return (&claw);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign left/right motor and potentiometer to the claw system.  */
/** @param[in]  leftMotor The left claw motor                                  */
/** @param[in]  leftPotentiometer The left claw potentiometer                  */
/** @param[in]  leftSensorReversed Is the left claw potentiometer reversed?    */
/** @param[in]  rightMotor The right claw motor                                */
/** @param[in]  rightPotentiometer The right claw potentiometer                */
/** @param[in]  rightSensorReversed Is the right claw potentiometer reversed?  */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  leftBackValue The left claw potentiometer back value           */
/** @param[in]  leftGrabValue The left claw potentiometer grab value           */
/** @param[in]  leftOpenValue The left claw potentiometer open value           */
/** @param[in]  rightBackValue The right claw potentiometer back value         */
/** @param[in]  rightGrabValue The right claw potentiometer grab value         */
/** @param[in]  rightOpenValue The right claw potentiometer open value         */
/*-----------------------------------------------------------------------------*/
void
clawSetup(tVexMotor leftMotor, tVexAnalogPin leftPotentiometer, bool_t leftSensorReversed,
		  tVexMotor rightMotor, tVexAnalogPin rightPotentiometer, bool_t rightSensorReversed,
		  float gearRatio, int16_t leftBackValue, int16_t leftGrabValue, int16_t leftOpenValue,
		  int16_t rightBackValue, int16_t rightGrabValue, int16_t rightOpenValue)
{
	claw.leftMotor = leftMotor;
	claw.leftPotentiometer = leftPotentiometer;
	claw.leftSensorReversed = leftSensorReversed;
	claw.rightMotor = rightMotor;
	claw.rightPotentiometer = rightPotentiometer;
	claw.rightSensorReversed = rightSensorReversed;
	claw.gearRatio = gearRatio;
	claw.leftBackValue = leftBackValue;
	claw.leftGrabValue = leftGrabValue;
	claw.leftOpenValue = leftOpenValue;
	claw.rightBackValue = rightBackValue;
	claw.rightGrabValue = rightGrabValue;
	claw.rightOpenValue = rightOpenValue;
	claw.leftLock = NULL;
	claw.rightLock = NULL;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the claw system.                                    */
/*-----------------------------------------------------------------------------*/
void
clawInit(void)
{
	SmartMotorSetRpmSensor(claw.leftMotor, claw.leftPotentiometer, 6000 * claw.gearRatio, claw.leftSensorReversed);
	SmartMotorSetRpmSensor(claw.rightMotor, claw.rightPotentiometer, 6000 * claw.gearRatio, claw.rightSensorReversed);
	// claw.lock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	// claw.lock->enabled = 0;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the claw system thread                                  */
/*-----------------------------------------------------------------------------*/
void
clawStart(void)
{
	chThdCreateStatic(waClaw, sizeof(waClaw), NORMALPRIO - 1, clawThread, NULL);
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      The claw system thread                                         */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
clawThread(void *arg)
{
	int16_t clawCmd = 0;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("claw");

	while (!chThdShouldTerminate()) {
		clawCmd = clawSpeed( vexControllerGet( Ch2 ) );
		SetMotor( claw.leftMotor, clawCmd );
		// SetMotor( claw.rightMotor, clawCmd );
		// if (vexControllerGet( Btn5UXmtr2 )) {
		// 	clawCmd = clawSpeed( vexControllerGet( Ch1Xmtr2 ) );
		// } else {
		// 	clawCmd = 0;
		// }
		// if (clawCmd == 0) {
		// 	// claw open and close
		// 	if (vexControllerGet( Btn6UXmtr2 )) {
		// 		claw.lock->enabled = 1;
		// 		claw.lock->target_value = claw.grabValue;
		// 	} else if (vexControllerGet( Btn6DXmtr2 ) || vexControllerGet( Btn7DXmtr2 )) {
		// 		claw.lock->enabled = 1;
		// 		claw.lock->target_value = claw.openValue;
		// 	}
		// 	clawPIDUpdate(&clawCmd);
		// } else {
		// 	// disable PID if driving
		// 	claw.lock->enabled = 0;
		// 	PidControllerUpdate( claw.lock ); // zero out PID
		// }

		// SetMotor( claw.motor, clawCmd );

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
clawPIDUpdate(int16_t *cmd)
{
	// // enable PID if not driving and already disabled
	// if (claw.lock->enabled == 0) {
	// 	claw.lock->enabled = 1;
	// 	claw.lock->target_value = vexAdcGet( claw.potentiometer );
	// }
	// // prevent PID from trying to lock outside bounds
	// if (claw.lock->target_value > claw.grabValue)
	// 	claw.lock->target_value = claw.grabValue;
	// else if (claw.lock->target_value < claw.openValue)
	// 	claw.lock->target_value = claw.openValue;
	// // update PID
	// claw.lock->sensor_value = vexAdcGet( claw.potentiometer );
	// claw.lock->error =
	// 	(claw.reversed)
	// 	? (claw.lock->sensor_value - claw.lock->target_value)
	// 	: (claw.lock->target_value - claw.lock->sensor_value);
	// *cmd = PidControllerUpdate( claw.lock );
	// // limit output if error is small
	// if (fabs(claw.lock->error) < 50) {
	// 	*cmd = *cmd / 2;
	// }
	// *cmd = clawSpeed( *cmd );
	return;
}
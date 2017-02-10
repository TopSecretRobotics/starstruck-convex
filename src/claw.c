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
static void		clawPIDSync(void);
static void		clawPIDUpdate(int16_t *leftCmd, int16_t *rightCmd);

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
/** @param[in]  rightMotor The right claw motor                                */
/** @param[in]  potentiometer The claw potentiometer                           */
/** @param[in]  sensorReversed Is the claw potentiometer reversed?             */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  grabValue The claw potentiometer grab value                    */
/** @param[in]  openValue The claw potentiometer open value                    */
/*-----------------------------------------------------------------------------*/
void
clawSetup(tVexMotor leftMotor, tVexMotor rightMotor, tVexAnalogPin potentiometer,
		  bool_t sensorReversed, float gearRatio, int16_t grabValue, int16_t openValue)
{
	claw.leftMotor = leftMotor;
	claw.rightMotor = rightMotor;
	claw.potentiometer = potentiometer;
	claw.sensorReversed = sensorReversed;
	claw.gearRatio = gearRatio;
	claw.grabValue = grabValue;
	claw.openValue = openValue;
	claw.leftLock = NULL;
	claw.rightLock = NULL;
	claw.isGrabbing = FALSE;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the claw system.                                    */
/*-----------------------------------------------------------------------------*/
void
clawInit(void)
{
	// SmartMotorSetRpmSensor(claw.leftMotor, claw.leftPotentiometer, 6000 * claw.gearRatio, claw.leftSensorReversed);
	// SmartMotorSetRpmSensor(claw.rightMotor, claw.rightPotentiometer, 6000 * claw.gearRatio, claw.rightSensorReversed);
	claw.leftLock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	claw.leftLock->enabled = 0;
	// claw.rightLock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	claw.rightLock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	claw.rightLock->enabled = 0;
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
	int16_t leftClawCmd = 0;
	int16_t rightClawCmd = 0;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("claw");

	while (!chThdShouldTerminate()) {
		clawCmd = clawSpeed( vexControllerGet( Ch2 ) );
		//clawCmd = 0;
		leftClawCmd = rightClawCmd = clawCmd;
		if (clawCmd == 0) {
			// claw open and grab
			if (vexControllerGet( Btn6U ) || vexControllerGet( Btn6UXmtr2 )) {
				claw.isGrabbing = TRUE;
				claw.leftLock->enabled = 1;
				claw.leftLock->target_value = claw.grabValue;
				claw.rightLock->enabled = 1;
				claw.rightLock->target_value = claw.grabValue;
			} else if (vexControllerGet( Btn6D ) || vexControllerGet( Btn6DXmtr2 )) {
				claw.isGrabbing = FALSE;
				claw.leftLock->enabled = 1;
				claw.leftLock->target_value = claw.openValue;
				claw.rightLock->enabled = 1;
				claw.rightLock->target_value = claw.openValue;
			}
			clawPIDUpdate(&leftClawCmd, &rightClawCmd);
		} else {
			claw.isGrabbing = FALSE;
			claw.leftLock->enabled = 0;
			claw.rightLock->enabled = 0;
			claw.leftLock->target_value = claw.rightLock->target_value = vexAdcGet( claw.potentiometer );
			PidControllerUpdate( claw.leftLock ); // zero out left PID
			PidControllerUpdate( claw.rightLock ); // zero out right PID
			// If claw is already grab or open, don't allow the motors to break the claw.
			if ((leftClawCmd < 0 || rightClawCmd < 0) &&
					((claw.leftLock->target_value >= (claw.openValue - 500)) || (claw.rightLock->target_value >= (claw.openValue - 500)))) {
				leftClawCmd = rightClawCmd = 0;
			} else if ((leftClawCmd > 0 || rightClawCmd > 0) &&
					((claw.leftLock->target_value <= (claw.grabValue + 500)) || (claw.rightLock->target_value <= (claw.grabValue + 500)))) {
				leftClawCmd = rightClawCmd = 0;
			}
		}
		SetMotor( claw.leftMotor, leftClawCmd );
		SetMotor( claw.rightMotor, rightClawCmd );

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
clawPIDUpdate(int16_t *leftCmd, int16_t *rightCmd)
{
	// enable PID if not driving and already disabled
	if (claw.leftLock->enabled == 0 || claw.rightLock->enabled == 0) {
		claw.leftLock->enabled = 1;
		claw.leftLock->target_value = vexAdcGet( claw.potentiometer );
		claw.rightLock->enabled = 1;
		claw.rightLock->target_value = vexAdcGet( claw.potentiometer );
	}
	// prevent PID from trying to lock outside bounds
	if (claw.leftLock->target_value < claw.grabValue)
		claw.leftLock->target_value = claw.grabValue;
	else if (claw.leftLock->target_value > claw.openValue)
		claw.leftLock->target_value = claw.openValue;
	if (claw.rightLock->target_value < claw.grabValue)
		claw.rightLock->target_value = claw.grabValue;
	else if (claw.rightLock->target_value > claw.openValue)
		claw.rightLock->target_value = claw.openValue;
	// update PID
	claw.leftLock->sensor_value = vexAdcGet( claw.potentiometer );
	claw.leftLock->error =
		(claw.sensorReversed)
		? (claw.leftLock->sensor_value - claw.leftLock->target_value)
		: (claw.leftLock->target_value - claw.leftLock->sensor_value);
	*leftCmd = PidControllerUpdate( claw.leftLock );
	claw.rightLock->sensor_value = vexAdcGet( claw.potentiometer );
	claw.rightLock->error =
		(claw.sensorReversed)
		? (claw.rightLock->sensor_value - claw.rightLock->target_value)
		: (claw.rightLock->target_value - claw.rightLock->sensor_value);
	*rightCmd = PidControllerUpdate( claw.rightLock );
	// limit output if error is small
	if (fabs(claw.leftLock->error) > 500) {
		*leftCmd = *leftCmd * 10;
	} else if (fabs(claw.leftLock->error) < 100) {
		*leftCmd = *leftCmd / 5;
	}
	// if (fabs(claw.leftLock->error) < 100) {
	// 	*leftCmd = *leftCmd / 5;
	// } else if (!claw.isGrabbing && fabs(claw.leftLock->error) < 500) {
	// 	*leftCmd = *leftCmd / 3;
	// } else if (!claw.isGrabbing && fabs(claw.leftLock->error) < 2000) {
	// 	*leftCmd = *leftCmd / 2;
	// }
	*leftCmd = clawSpeed( *leftCmd );
	if (fabs(claw.rightLock->error) > 500) {
		*rightCmd = *rightCmd * 10;
	} else if (fabs(claw.rightLock->error) < 100) {
		*rightCmd = *rightCmd / 5;
	}
	// if (fabs(claw.rightLock->error) < 100) {
	// 	*rightCmd = *rightCmd / 5;
	// } else if (!claw.isGrabbing && fabs(claw.rightLock->error) < 500) {
	// 	*rightCmd = *rightCmd / 3;
	// } else if (!claw.isGrabbing && fabs(claw.rightLock->error) < 2000) {
	// 	*rightCmd = *rightCmd / 2;
	// }
	*rightCmd = clawSpeed( *rightCmd );
	return;
}

void
clawMove(int16_t cmd, bool_t immediate)
{
	SetMotor( claw.leftMotor,  cmd, immediate );
	SetMotor( claw.rightMotor, cmd, immediate );
}
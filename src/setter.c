/*-----------------------------------------------------------------------------*/
/** @file    setter.c                                                            */
/** @brief   The setter system for the robot                                     */
/*-----------------------------------------------------------------------------*/

#include "setter.h"

#include <math.h>
#include <stdlib.h>

// storage for setter
static setter_t setter;

// working area for setter task
static WORKING_AREA(waSetter, 512);

// private functions
static msg_t	setterThread(void *arg);
static void		setterPIDUpdate(int16_t *leftCmd, int16_t *rightCmd);

// setter speed adjustment
#define USE_SETTER_SPEED_TABLE 1
#ifdef USE_SETTER_SPEED_TABLE

const unsigned int setterSpeedTable[128] =
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
setterSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (((speed > 0) - (speed < 0)) * setterSpeedTable[abs(speed)]);
}

#else

static inline int
setterSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (speed);
}

#endif

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to setter structure - not used locally               */
/** @return     A setter_t pointer                                               */
/*-----------------------------------------------------------------------------*/
setter_t *
setterGetPtr(void)
{
	return (&setter);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign left/right motor and potentiometer to the setter system.  */
/** @param[in]  leftMotor The left setter motor                                  */
/** @param[in]  rightMotor The right setter motor                                */
/** @param[in]  potentiometer The setter potentiometer                           */
/** @param[in]  sensorReversed Is the setter potentiometer reversed?             */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  grabValue The setter potentiometer grab value                    */
/** @param[in]  openValue The setter potentiometer open value                    */
/*-----------------------------------------------------------------------------*/
void
setterSetup(tVexMotor leftMotor, tVexMotor rightMotor, tVexAnalogPin potentiometer,
		  bool_t sensorReversed, float gearRatio, int16_t grabValue, int16_t openValue)
{
	setter.leftMotor = leftMotor;
	setter.rightMotor = rightMotor;
	setter.potentiometer = potentiometer;
	setter.sensorReversed = sensorReversed;
	setter.gearRatio = gearRatio;
	setter.grabValue = grabValue;
	setter.openValue = openValue;
	setter.locked = TRUE;
	setter.leftLock = NULL;
	setter.rightLock = NULL;
	setter.isGrabbing = FALSE;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the setter system.                                    */
/*-----------------------------------------------------------------------------*/
void
setterInit(void)
{
	// SmartMotorSetRpmSensor(setter.leftMotor, setter.leftPotentiometer, 6000 * setter.gearRatio, setter.leftSensorReversed);
	// SmartMotorSetRpmSensor(setter.rightMotor, setter.rightPotentiometer, 6000 * setter.gearRatio, setter.rightSensorReversed);
	setter.leftLock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	setter.leftLock->enabled = 0;
	// setter.rightLock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	setter.rightLock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	setter.rightLock->enabled = 0;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the setter system thread                                  */
/*-----------------------------------------------------------------------------*/
void
setterStart(void)
{
	chThdCreateStatic(waSetter, sizeof(waSetter), NORMALPRIO - 1, setterThread, NULL);
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      The setter system thread                                         */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
setterThread(void *arg)
{
	int16_t setterCmd = 0;
	int16_t leftSetterCmd = 0;
	int16_t rightSetterCmd = 0;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("setter");

	while (!chThdShouldTerminate()) {
		if (setter.locked) {
			setterCmd = setterSpeed( vexControllerGet( Ch2 ) );
			//setterCmd = 0;
			leftSetterCmd = rightSetterCmd = setterCmd;
			if (setterCmd == 0) {
				// setter open and grab
				if (vexControllerGet( Btn6U ) || vexControllerGet( Btn6UXmtr2 )) {
					setter.isGrabbing = TRUE;
					setter.leftLock->enabled = 1;
					setter.leftLock->target_value = setter.grabValue;
					setter.rightLock->enabled = 1;
					setter.rightLock->target_value = setter.grabValue;
				} else if (vexControllerGet( Btn6D ) || vexControllerGet( Btn6DXmtr2 )) {
					setter.isGrabbing = FALSE;
					setter.leftLock->enabled = 1;
					setter.leftLock->target_value = setter.openValue;
					setter.rightLock->enabled = 1;
					setter.rightLock->target_value = setter.openValue;
				}
				setterPIDUpdate(&leftSetterCmd, &rightSetterCmd);
			} else {
				setter.isGrabbing = FALSE;
				setter.leftLock->enabled = 0;
				setter.rightLock->enabled = 0;
				setter.leftLock->target_value = setter.rightLock->target_value = vexAdcGet( setter.potentiometer );
				PidControllerUpdate( setter.leftLock ); // zero out left PID
				PidControllerUpdate( setter.rightLock ); // zero out right PID
				// If setter is already grab or open, don't allow the motors to break the setter.
				if ((leftSetterCmd < 0 || rightSetterCmd < 0) &&
						((setter.leftLock->target_value >= (setter.openValue - 250)) || (setter.rightLock->target_value >= (setter.openValue - 250)))) {
					leftSetterCmd = rightSetterCmd = 0;
				} else if ((leftSetterCmd > 0 || rightSetterCmd > 0) &&
						((setter.leftLock->target_value <= (setter.grabValue + 250)) || (setter.rightLock->target_value <= (setter.grabValue + 250)))) {
					leftSetterCmd = rightSetterCmd = 0;
				}
			}
			SetMotor( setter.leftMotor, leftSetterCmd );
			SetMotor( setter.rightMotor, rightSetterCmd );
		}

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
setterPIDUpdate(int16_t *leftCmd, int16_t *rightCmd)
{
	// enable PID if not driving and already disabled
	if (setter.leftLock->enabled == 0 || setter.rightLock->enabled == 0) {
		setter.leftLock->enabled = 1;
		setter.leftLock->target_value = vexAdcGet( setter.potentiometer );
		setter.rightLock->enabled = 1;
		setter.rightLock->target_value = vexAdcGet( setter.potentiometer );
	}
	// prevent PID from trying to lock outside bounds
	if (setter.leftLock->target_value < setter.grabValue)
		setter.leftLock->target_value = setter.grabValue;
	else if (setter.leftLock->target_value > setter.openValue)
		setter.leftLock->target_value = setter.openValue;
	if (setter.rightLock->target_value < setter.grabValue)
		setter.rightLock->target_value = setter.grabValue;
	else if (setter.rightLock->target_value > setter.openValue)
		setter.rightLock->target_value = setter.openValue;
	// update PID
	setter.leftLock->sensor_value = vexAdcGet( setter.potentiometer );
	setter.leftLock->error =
		(setter.sensorReversed)
		? (setter.leftLock->sensor_value - setter.leftLock->target_value)
		: (setter.leftLock->target_value - setter.leftLock->sensor_value);
	*leftCmd = PidControllerUpdate( setter.leftLock );
	setter.rightLock->sensor_value = vexAdcGet( setter.potentiometer );
	setter.rightLock->error =
		(setter.sensorReversed)
		? (setter.rightLock->sensor_value - setter.rightLock->target_value)
		: (setter.rightLock->target_value - setter.rightLock->sensor_value);
	*rightCmd = PidControllerUpdate( setter.rightLock );
	// limit output if error is small
	if (fabs(setter.leftLock->error) > 500) {
		*leftCmd = *leftCmd * 10;
	} else if (fabs(setter.leftLock->error) < 100) {
		*leftCmd = *leftCmd / 5;
	}
	// if (fabs(setter.leftLock->error) < 100) {
	// 	*leftCmd = *leftCmd / 5;
	// } else if (!setter.isGrabbing && fabs(setter.leftLock->error) < 500) {
	// 	*leftCmd = *leftCmd / 3;
	// } else if (!setter.isGrabbing && fabs(setter.leftLock->error) < 2000) {
	// 	*leftCmd = *leftCmd / 2;
	// }
	*leftCmd = setterSpeed( *leftCmd );
	if (fabs(setter.rightLock->error) > 500) {
		*rightCmd = *rightCmd * 10;
	} else if (fabs(setter.rightLock->error) < 100) {
		*rightCmd = *rightCmd / 5;
	}
	// if (fabs(setter.rightLock->error) < 100) {
	// 	*rightCmd = *rightCmd / 5;
	// } else if (!setter.isGrabbing && fabs(setter.rightLock->error) < 500) {
	// 	*rightCmd = *rightCmd / 3;
	// } else if (!setter.isGrabbing && fabs(setter.rightLock->error) < 2000) {
	// 	*rightCmd = *rightCmd / 2;
	// }
	*rightCmd = setterSpeed( *rightCmd );
	return;
}

void
setterMove(int16_t cmd, bool_t immediate)
{
	SetMotor( setter.leftMotor,  cmd, immediate );
	SetMotor( setter.rightMotor, cmd, immediate );
}

void
setterLock(void)
{
	setter.locked = TRUE;
}

void
setterUnlock(void)
{
	setter.locked = FALSE;
}

void
setterLockGrab(void)
{
	setterLock();
	setter.isGrabbing = TRUE;
	setter.leftLock->enabled = 1;
	setter.leftLock->target_value = setter.grabValue;
	setter.rightLock->enabled = 1;
	setter.rightLock->target_value = setter.grabValue;
}

void
setterLockOpen(void)
{
	setterLock();
	setter.isGrabbing = FALSE;
	setter.leftLock->enabled = 1;
	setter.leftLock->target_value = setter.openValue;
	setter.rightLock->enabled = 1;
	setter.rightLock->target_value = setter.openValue;
}

void
setterLockCurrent(void)
{
	setterLock();
	setter.isGrabbing = FALSE;
	setter.leftLock->enabled = 1;
	setter.leftLock->target_value = vexAdcGet( setter.potentiometer );
	setter.rightLock->enabled = 1;
	setter.rightLock->target_value = vexAdcGet( setter.potentiometer setter
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
/** @brief      Assign motor and potentiometer to the claw system.             */
/** @param[in]  motor The claw motor                                           */
/** @param[in]  potentiometer The claw potentiometer                           */
/** @param[in]  reversed Is the claw potentiometer reversed?                   */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  grabValue The claw potentiometer grab value                    */
/** @param[in]  openValue The claw potentiometer open value                    */
/*-----------------------------------------------------------------------------*/
void
clawSetup(tVexMotor motor, tVexAnalogPin potentiometer, bool_t reversed, float gearRatio, int16_t grabValue, int16_t openValue)
{
	claw.motor = motor;
	claw.potentiometer = potentiometer;
	claw.reversed = reversed;
	claw.gearRatio = gearRatio;
	claw.grabValue = grabValue;
	claw.openValue = openValue;
	claw.lock = NULL;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the claw system.                                    */
/*-----------------------------------------------------------------------------*/
void
clawInit(void)
{
	SmartMotorSetRpmSensor(claw.motor, claw.potentiometer, 6000 * claw.gearRatio, claw.reversed);
	claw.lock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	claw.lock->enabled = 0;
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
		if (vexControllerGet( Btn5UXmtr2 )) {
			clawCmd = clawSpeed( vexControllerGet( Ch1Xmtr2 ) );
		} else {
			clawCmd = 0;
		}
		if (clawCmd == 0) {
			// claw open and close
			if (vexControllerGet( Btn6UXmtr2 )) {
				claw.lock->enabled = 1;
				claw.lock->target_value = claw.grabValue;
			} else if (vexControllerGet( Btn6DXmtr2 ) || vexControllerGet( Btn7DXmtr2 )) {
				claw.lock->enabled = 1;
				claw.lock->target_value = claw.openValue;
			}
			clawPIDUpdate(&clawCmd);
		} else {
			// disable PID if driving
			claw.lock->enabled = 0;
			PidControllerUpdate( claw.lock ); // zero out PID
		}

		SetMotor( claw.motor, clawCmd );

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
clawPIDUpdate(int16_t *cmd)
{
	// enable PID if not driving and already disabled
	if (claw.lock->enabled == 0) {
		claw.lock->enabled = 1;
		claw.lock->target_value = vexAdcGet( claw.potentiometer );
	}
	// prevent PID from trying to lock outside bounds
	if (claw.lock->target_value > claw.grabValue)
		claw.lock->target_value = claw.grabValue;
	else if (claw.lock->target_value < claw.openValue)
		claw.lock->target_value = claw.openValue;
	// update PID
	claw.lock->sensor_value = vexAdcGet( claw.potentiometer );
	claw.lock->error =
		(claw.reversed)
		? (claw.lock->sensor_value - claw.lock->target_value)
		: (claw.lock->target_value - claw.lock->sensor_value);
	*cmd = PidControllerUpdate( claw.lock );
	// limit output if error is small
	if (fabs(claw.lock->error) < 50) {
		*cmd = *cmd / 2;
	}
	*cmd = clawSpeed( *cmd );
	return;
}
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
/** @param[in]  restValue The arm potentiometer rest value                     */
/** @param[in]  restInvertedValue The arm potentiometer rest value (inverted)  */
/*-----------------------------------------------------------------------------*/
void
armSetup(tVexMotor topMotorPair, tVexMotor middleMotorPair, tVexMotor bottomMotorPair, tVexAnalogPin potentiometer, bool_t reversed, float gearRatio, int16_t restValue, int16_t restInvertedValue)
{
	arm.topMotorPair = topMotorPair;
	arm.middleMotorPair = middleMotorPair;
	arm.bottomMotorPair = bottomMotorPair;
	arm.potentiometer = potentiometer;
	arm.reversed = reversed;
	arm.gearRatio = gearRatio;
	arm.restValue = restValue;
	arm.restInvertedValue = restInvertedValue;
	arm.lock = NULL;
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
	// TODO: is this the correct way to link the motors?
	SmartMotorLinkMotors(arm.bottomMotorPair, arm.topMotorPair);
	SmartMotorLinkMotors(arm.bottomMotorPair, arm.middleMotorPair);
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

/*-----------------------------------------------------------------------------*/
/** @brief      The arm system thread                                          */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
armThread(void *arg)
{
	int16_t armCmd = 0;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("arm");

	while (!chThdShouldTerminate()) {
		armCmd = armSpeed( vexControllerGet( Ch1 ) );

		// (void) armPIDUpdate;

		if (armCmd == 0) {
			if (vexControllerGet( Btn7D )) {
				arm.lock->enabled = 1;
				arm.lock->target_value = arm.restValue;
			}
			armPIDUpdate(&armCmd);
			// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "error %f", arm.lock->error);
		} else {
			// disable PID if driving
			arm.lock->enabled = 0;
			PidControllerUpdate( arm.lock ); // zero out PID
		}

		SetMotor( arm.topMotorPair, armCmd );
		SetMotor( arm.middleMotorPair, armCmd );
		SetMotor( arm.bottomMotorPair, armCmd );

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
	// if (arm.reversed) {
	// 	if (arm.lock->target_value > arm.restValue)
	// 		arm.lock->target_value = arm.restValue;
	// 	else if (arm.lock->target_value < arm.restInvertedValue)
	// 		arm.lock->target_value = arm.restInvertedValue;
	// } else {
	// 	if (arm.lock->target_value < arm.restValue)
	// 		arm.lock->target_value = arm.restValue;
	// 	else if (arm.lock->target_value > arm.restInvertedValue)
	// 		arm.lock->target_value = arm.restInvertedValue;
	// }
	if (arm.lock->target_value > arm.restValue)
		arm.lock->target_value = arm.restValue;
	else if (arm.lock->target_value < arm.restInvertedValue)
		arm.lock->target_value = arm.restInvertedValue;
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
	if (fabs(arm.lock->error) < 700) {
		*cmd = *cmd / 2;
	}
	*cmd = armSpeed( *cmd );
	return;
}

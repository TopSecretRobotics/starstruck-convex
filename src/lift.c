/*-----------------------------------------------------------------------------*/
/** @file    lift.c                                                             */
/** @brief   The lift system for the robot                                      */
/*-----------------------------------------------------------------------------*/

#include "lift.h"

#include <math.h>
#include <stdlib.h>

// storage for lift
static lift_t lift;

// working area for lift task
static WORKING_AREA(waLift, 512);

// private functions
static msg_t	liftThread(void *arg);
static void		liftPIDUpdate(int16_t *cmd);

// lift speed adjustment
#define USE_LIFT_SPEED_TABLE 1
#ifdef USE_LIFT_SPEED_TABLE

const unsigned int liftSpeedTable[128] =
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
liftSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (((speed > 0) - (speed < 0)) * liftSpeedTable[abs(speed)]);
}

#else

static inline int
liftSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (speed);
}

#endif

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to lift structure - not used locally               */
/** @return     A lift_t pointer                                               */
/*-----------------------------------------------------------------------------*/
lift_t *
liftGetPtr(void)
{
	return (&lift);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign motor and potentiometer to the lift system.              */
/** @param[in]  topMotorPair The lift top motor pair                            */
/** @param[in]  middleMotorPair The lift middle motor pair                      */
/** @param[in]  bottomMotorPair The lift bottom motor pair                      */
/** @param[in]  potentiometer The lift potentiometer                            */
/** @param[in]  reversed Is the lift potentiometer reversed?                    */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  downValue The lift potentiometer down value                     */
/** @param[in]  bumpValue The lift potentiometer bump value                     */
/** @param[in]  upValue The lift potentiometer up value                         */
/*-----------------------------------------------------------------------------*/
void
liftSetup(tVexMotor motor0, tVexMotor motor1, tVexMotor motor2,
		 tVexAnalogPin potentiometer, bool_t reversed, float gearRatio,
		 int16_t downValue, int16_t bumpValue, int16_t upValue)
{
	lift.motor0 = motor0;
	lift.motor1 = motor1;
	lift.motor2 = motor2;
	lift.potentiometer = potentiometer;
	lift.reversed = reversed;
	lift.gearRatio = gearRatio;
	lift.downValue = downValue;
	lift.bumpValue = bumpValue;
	lift.upValue = upValue;
	lift.position = liftPositionUnknown;
	lift.locked = TRUE;
	lift.lock = NULL;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the lift system.                                     */
/*-----------------------------------------------------------------------------*/
void
liftInit(void)
{
	// SmartMotorSetRpmSensor(lift.topMotorPair, lift.potentiometer, 6000 * lift.gearRatio, lift.reversed);
	// SmartMotorSetRpmSensor(lift.middleMotorPair, lift.potentiometer, 6000 * lift.gearRatio, lift.reversed);
	// SmartMotorSetRpmSensor(lift.bottomMotorPair, lift.potentiometer, 6000 * lift.gearRatio, lift.reversed);
	SmartMotorLinkMotors(lift.motor2, lift.motor1);
	SmartMotorLinkMotors(lift.motor2, lift.motor0);
	lift.lock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	lift.lock->enabled = 0;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the lift system thread                                    */
/*-----------------------------------------------------------------------------*/
void
liftStart(void)
{
	chThdCreateStatic(waLift, sizeof(waLift), NORMALPRIO - 1, liftThread, NULL);
	return;
}

#define IMMEDIATE_TIMEOUT 1000

static int
limitSpeed(int speed, int limit)
{
	if (abs(speed) <= limit) {
		return 0;
	}
	return speed;
}

/*-----------------------------------------------------------------------------*/
/** @brief      The lift system thread                                          */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
liftThread(void *arg)
{
	int16_t liftCmd = 0;
	bool_t immediate = FALSE;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("lift");

	while (!chThdShouldTerminate()) {
		if (lift.locked) {
			liftCmd = liftSpeed( limitSpeed( vexControllerGet( Ch2Xmtr2 ), 20 ) );

			if (liftCmd == 0) {
				immediate = FALSE;
				if (vexControllerGet( Btn7D ) || vexControllerGet( Btn7DXmtr2 )) {
					lift.position = liftPositionDown;
					lift.lock->enabled = 1;
					lift.lock->target_value = lift.downValue;
				} else if (vexControllerGet( Btn7L ) || vexControllerGet( Btn7LXmtr2 )) {
					lift.position = liftPositionBump;
					lift.lock->enabled = 1;
					lift.lock->target_value = lift.bumpValue;
				} else if (vexControllerGet( Btn7U ) || vexControllerGet( Btn7UXmtr2 )) {
					lift.position = liftPositionUp;
					lift.lock->enabled = 1;
					lift.lock->target_value = lift.upValue;
				}
				liftPIDUpdate(&liftCmd);
			} else {
				lift.position = liftPositionUnknown;
				immediate = TRUE;
				// disable PID if joystick driving
				lift.lock->enabled = 0;
				PidControllerUpdate( lift.lock ); // zero out PID
			}

			liftMove( liftCmd, immediate );
		}

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
liftPIDUpdate(int16_t *cmd)
{
	// enable PID if not driving and already disabled
	if (lift.lock->enabled == 0) {
		lift.lock->enabled = 1;
		lift.lock->target_value = vexAdcGet( lift.potentiometer );
	}
	// prevent PID from trying to lock outside bounds
	if (lift.reversed) {
		if (lift.lock->target_value > lift.downValue)
			lift.lock->target_value = lift.downValue;
		else if (lift.lock->target_value < lift.upValue)
			lift.lock->target_value = lift.upValue;
	} else {
		if (lift.lock->target_value < lift.downValue)
			lift.lock->target_value = lift.downValue;
		else if (lift.lock->target_value > lift.upValue)
			lift.lock->target_value = lift.upValue;
	}
	// update PID
	lift.lock->sensor_value = vexAdcGet( lift.potentiometer );
	lift.lock->error =
		(lift.reversed)
		? (lift.lock->sensor_value - lift.lock->target_value)
		: (lift.lock->target_value - lift.lock->sensor_value);
	*cmd = PidControllerUpdate( lift.lock );
	// // adjust output if down or up position requested
	// if (lift.position == liftPositionDown && fabs(lift.lock->error) > 50) {
	// 	*cmd = *cmd * 100;
	// } else if (lift.position == liftPositionUp && fabs(lift.lock->error) > 50) {
	// 	*cmd = *cmd * 100;
	// }
	*cmd = liftSpeed( *cmd );
	return;
}

void
liftMove(int16_t cmd, bool_t immediate)
{
	SetMotor( lift.motor0, cmd, immediate );
	SetMotor( lift.motor1, cmd, immediate );
	SetMotor( lift.motor2, cmd, immediate );
}

void
liftLock(void)
{
	lift.locked = TRUE;
}

void
liftUnlock(void)
{
	lift.locked = FALSE;
}

void
liftLockDown(void)
{
	liftLock();
	lift.position = liftPositionDown;
	lift.lock->enabled = 1;
	lift.lock->target_value = lift.downValue;
}

void
liftLockBump(void)
{
	liftLock();
	lift.position = liftPositionBump;
	lift.lock->enabled = 1;
	lift.lock->target_value = lift.bumpValue;
}

void
liftLockUp(void)
{
	liftLock();
	lift.position = liftPositionUp;
	lift.lock->enabled = 1;
	lift.lock->target_value = lift.upValue;
}

void
liftLockCurrent(void)
{
	liftLock();
	lift.position = liftPositionUnknown;
	lift.lock->enabled = 1;
	lift.lock->target_value = vexAdcGet( lift.potentiometer );
}

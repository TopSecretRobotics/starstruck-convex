/*-----------------------------------------------------------------------------*/
/** @file    wrist.c                                                           */
/** @brief   The wrist system for the robot                                    */
/*-----------------------------------------------------------------------------*/

#include "wrist.h"

#include <math.h>
#include <stdlib.h>

// storage for wrist
static wrist_t wrist;

// working area for wrist task
static WORKING_AREA(waWrist, 512);

// private functions
static msg_t	wristThread(void *arg);
static void		wristPIDUpdate(int16_t *cmd);
// static void		wristAutotune(void);

// wrist speed adjustment
#define USE_WRIST_SPEED_TABLE 1
#ifdef USE_WRIST_SPEED_TABLE

const unsigned int wristSpeedTable[128] =
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
wristSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (((speed > 0) - (speed < 0)) * wristSpeedTable[abs(speed)]);
}

#else

static inline int
wristSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (speed);
}

#endif

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to wrist structure - not used locally              */
/** @return     A wrist_t pointer                                              */
/*-----------------------------------------------------------------------------*/
wrist_t *
wristGetPtr(void)
{
	return (&wrist);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign motor and potentiometer to the wrist system.            */
/** @param[in]  motor The wrist motor                                          */
/** @param[in]  potentiometer The wrist potentiometer                          */
/** @param[in]  reversed Is the wrist potentiometer reversed?                  */
/** @param[in]  gearRatio Gear ratio between motor and potentiometer           */
/** @param[in]  downFront The wrist potentiometer down-front value             */
/** @param[in]  upBack The wrist potentiometer up-back value                   */
/** @param[in]  upFront The wrist potentiometer up-front value                 */
/*-----------------------------------------------------------------------------*/
void
wristSetup(tVexMotor motor, tVexAnalogPin potentiometer, bool_t reversed,
		   float gearRatio, int16_t downFront, int16_t upBack, int16_t upFront)
{
	wrist.motor = motor;
	wrist.potentiometer = potentiometer;
	wrist.reversed = reversed;
	wrist.gearRatio = gearRatio;
	wrist.downFront = downFront;
	wrist.upBack = upBack;
	wrist.upFront = upFront;
	wrist.lock = NULL;
	wrist.isDown = FALSE;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the wrist system.                                   */
/*-----------------------------------------------------------------------------*/
void
wristInit(void)
{
	// SmartMotorSetRpmSensor(wrist.motor, wrist.potentiometer, 6000 * wrist.gearRatio, wrist.reversed);
	wrist.lock = PidControllerInit(0.004, 0.0001, 0.01, kVexSensorUndefined, 0);
	wrist.lock->enabled = 0;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the wrist system thread                                  */
/*-----------------------------------------------------------------------------*/
void
wristStart(void)
{
	chThdCreateStatic(waWrist, sizeof(waWrist), NORMALPRIO - 1, wristThread, NULL);
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      The wrist system thread                                        */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
wristThread(void *arg)
{
	int16_t wristCmd = 0;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("wrist");

	// wristAutotune();

	while (!chThdShouldTerminate()) {
		// if (!vexControllerGet( Btn5UXmtr2 )) {
		// 	wristCmd = wristSpeed( vexControllerGet( Ch2Xmtr2 ) );
		// } else {
		// 	wristCmd = 0;
		// }
		wristCmd = wristSpeed( vexControllerGet( Ch3Xmtr2 ) );
		if (wristCmd == 0) {
			if (vexControllerGet( Btn7D ) || vexControllerGet( Btn7DXmtr2 )) {
				wrist.isDown = TRUE;
				wrist.lock->enabled = 1;
				wrist.lock->target_value = wrist.downFront;
			} else if (vexControllerGet( Btn7U ) || vexControllerGet( Btn7UXmtr2 )) {
				wrist.isDown = FALSE;
				wrist.lock->enabled = 1;
				wrist.lock->target_value = wrist.upBack;
			// } else if (vexControllerGet( Btn7R )) {
			// 	wrist.lock->enabled = 1;
			// 	wrist.lock->target_value = wrist.upFront;
			}
			wristPIDUpdate(&wristCmd);
			// wristCmd = wristSpeed( PidControllerUpdate( wrist.lock ) );
			// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "error %f", wrist.lock->error);
		} else {
			// disable PID if driving
			wrist.lock->enabled = 0;
			PidControllerUpdate( wrist.lock ); // zero out PID
		}

		SetMotor( wrist.motor, wristCmd );

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

static void
wristPIDUpdate(int16_t *cmd)
{
	// enable PID if not driving and already disabled
	if (wrist.lock->enabled == 0) {
		wrist.lock->enabled = 1;
		wrist.lock->target_value = vexAdcGet( wrist.potentiometer );
	}
	// prevent PID from trying to lock outside bounds
	if (wrist.lock->target_value > wrist.downFront)
		wrist.lock->target_value = wrist.downFront;
	else if (wrist.lock->target_value < wrist.upFront)
		wrist.lock->target_value = wrist.upFront;
	// update PID
	wrist.lock->sensor_value = vexAdcGet( wrist.potentiometer );
	wrist.lock->error =
		(wrist.reversed)
		? (wrist.lock->sensor_value - wrist.lock->target_value)
		: (wrist.lock->target_value - wrist.lock->sensor_value);
	*cmd = PidControllerUpdate( wrist.lock );
	// limit output if error is small
	if (wrist.isDown) {
		*cmd = *cmd / 1;
	// } else if (fabs(wrist.lock->error) < 100) {
	// 	*cmd = *cmd / 10;
	// } else if (fabs(wrist.lock->error) < 300) {
	// 	*cmd = *cmd / 5;
	// } else if (fabs(wrist.lock->error) < 700) {
	// 	*cmd = *cmd / 2;
	}
	*cmd = wristSpeed( *cmd );
	return;
}

// static void
// wristAutotune(void)
// {
// 	autotuner_t *at = &(wrist.at);
// 	autotuneInit(at, (float) vexAdcGet( wrist.potentiometer ), 0, wrist.reversed);
// 	at->controlType = 1;
// 	at->setpoint = 1970;
// 	at->noiseBand = 20;
// 	at->oStep = 30;
// 	autotuneSetLookbackSec(at, 30);
// 	// bool_t tuning = FALSE;
// 	bool_t tuning = TRUE;
// 	while (tuning) {
// 		at->input = (float) vexAdcGet( wrist.potentiometer );
// 		if (autotuneRuntime(at)) {
// 			tuning = FALSE;
// 		}
// 		if (!tuning) {
// 			wrist.lock->Kp = autotuneGetKp(at);
// 			wrist.lock->Ki = autotuneGetKi(at);
// 			wrist.lock->Kd = autotuneGetKd(at);
// 		}
// 		SetMotor( wrist.motor, (int) at->output );
// 		vexSleep(25);
// 	}

// 	wrist.lock->enabled = 1;
// 	wrist.lock->target_value = 1970;

// 	return;
// }

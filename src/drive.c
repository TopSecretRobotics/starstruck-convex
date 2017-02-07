/*-----------------------------------------------------------------------------*/
/** @file    drive.c                                                           */
/** @brief   The drive system for the robot                                    */
/*-----------------------------------------------------------------------------*/

#include "drive.h"
#include <math.h>
#include <stdlib.h>

// storage for drive
static drive_t drive;

// working area for drive task
static WORKING_AREA(waDrive, 512);

// private functions
static msg_t	driveThread(void *arg);

// drive speed adjustment
#define USE_DRIVE_SPEED_TABLE 1
#ifdef USE_DRIVE_SPEED_TABLE

const unsigned int driveSpeedTable[128] =
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
driveSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (((speed > 0) - (speed < 0)) * driveSpeedTable[abs(speed)]);
}

#else

static inline int
driveSpeed(int speed)
{
	if (speed > 127)
		speed = 127;
	else if (speed < -127)
		speed = -127;
	return (speed);
}

#endif

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to drive structure - not used locally              */
/** @return     A drive_t pointer                                              */
/*-----------------------------------------------------------------------------*/
drive_t *
driveGetPtr(void)
{
	return (&drive);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign motors to each of the 4 wheels of the X-drive system.   */
/** @param[in]  northeast The front-right motor                                */
/** @param[in]  northwest The front-left motor                                 */
/** @param[in]  northeast The back-right motor                                 */
/** @param[in]  northwest The back-left motor                                  */
/*-----------------------------------------------------------------------------*/
void
driveSetup(tVexMotor northeast, tVexMotor northwest, tVexMotor southeast, tVexMotor southwest)
{
	drive.northeast = northeast;
	drive.northwest = northwest;
	drive.southeast = southeast;
	drive.southwest = southwest;
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the drive system.                                   */
/*-----------------------------------------------------------------------------*/
void
driveInit(void)
{
	SmartMotorSetSlewRate(drive.southeast, 50);
	SmartMotorSetSlewRate(drive.southwest, 50);
	// SmartMotorSetLimitCurent(drive.southeast, 3.0);
	// SmartMotorSetLimitCurent(drive.southwest, 3.0);
	SmartMotorLinkMotors(drive.southeast, drive.northeast);
	SmartMotorLinkMotors(drive.southwest, drive.northwest);
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the drive system thread                                  */
/*-----------------------------------------------------------------------------*/
void
driveStart(void)
{
	chThdCreateStatic(waDrive, sizeof(waDrive), NORMALPRIO - 1, driveThread, NULL);
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      The drive system thread                                        */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
driveThread(void *arg)
{
	int16_t driveX = 0;
	int16_t driveY = 0;
	// int16_t driveR = 0;
	// bool_t immediate = TRUE;
	bool_t immediate = FALSE;

	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("drive");

	while (!chThdShouldTerminate()) {
		driveX = driveSpeed( vexControllerGet( Ch4 ) );
		driveY = driveSpeed( vexControllerGet( Ch3 ) );
		// driveR = vexControllerGet( Ch1 ) + vexControllerGet( Ch1Xmtr2 );

		SetMotor( drive.northeast, driveSpeed( driveY - driveX ), immediate );
		SetMotor( drive.northwest, driveSpeed( driveY + driveX ), immediate );
		SetMotor( drive.southeast, driveSpeed( driveY - driveX ), immediate );
		SetMotor( drive.southwest, driveSpeed( driveY + driveX ), immediate );

		// SetMotor( drive.northeast, driveSpeed( driveY - driveX - driveR ) );
		// SetMotor( drive.northwest, driveSpeed( driveY + driveX + driveR ) );
		// SetMotor( drive.southeast, driveSpeed( driveY + driveX - driveR ) );
		// SetMotor( drive.southwest, driveSpeed( driveY - driveX + driveR ) );

		// Don't hog cpu
		vexSleep(25);
	}

	return ((msg_t) 0);
}

/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2013                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     vexuser.c                                                    */
/*    Author:     James Pearman                                                */
/*    Created:    7 May 2013                                                   */
/*                                                                             */
/*    Revisions:                                                               */
/*                V1.00  04 July 2013 - Initial release                        */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    The author is supplying this software for use with the VEX cortex        */
/*    control system. This file can be freely distributed and teams are        */
/*    authorized to freely use this program , however, it is requested that    */
/*    improvements or additions be shared with the Vex community via the vex   */
/*    forum.  Please acknowledge the work of the authors when appropriate.     */
/*    Thanks.                                                                  */
/*                                                                             */
/*    Licensed under the Apache License, Version 2.0 (the "License");          */
/*    you may not use this file except in compliance with the License.         */
/*    You may obtain a copy of the License at                                  */
/*                                                                             */
/*      http://www.apache.org/licenses/LICENSE-2.0                             */
/*                                                                             */
/*    Unless required by applicable law or agreed to in writing, software      */
/*    distributed under the License is distributed on an "AS IS" BASIS,        */
/*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*    See the License for the specific language governing permissions and      */
/*    limitations under the License.                                           */
/*                                                                             */
/*    The author can be contacted on the vex forums as jpearman                */
/*    or electronic mail using jbpearman_at_mac_dot_com                        */
/*    Mentor for team 8888 RoboLancers, Pasadena CA.                           */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#include <stdlib.h>

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "smartmotor.h"
#include "drive.h"
#include "arm.h"
#include "wrist.h"
#include "claw.h"

// Digital I/O configuration
static vexDigiCfg dConfig[kVexDigital_Num] = {
	{ kVexDigital_1,	kVexSensorDigitalOutput,	kVexConfigOutput,		0 },
	{ kVexDigital_2,	kVexSensorDigitalOutput,	kVexConfigOutput,		0 },
	{ kVexDigital_3,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_4,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_5,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_6,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_7,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_8,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_9,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_10,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_11,	kVexSensorDigitalInput,		kVexConfigInput,		0 },
	{ kVexDigital_12,	kVexSensorDigitalInput,		kVexConfigInput,		0 }
};

// Motor configuration
static vexMotorCfg mConfig[kVexMotorNum] = {
	{ kVexMotor_1,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_2,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_3,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_4,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_5,		kVexMotorUndefined,		kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_6,		kVexMotor393T,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_1 },
	{ kVexMotor_7,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_8,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_9,		kVexMotor393T,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_10,		kVexMotor393T,			kVexMotorReversed,		kVexSensorNone,			0 }
};

/*-----------------------------------------------------------------------------*/
/** @brief      User setup                                                     */
/*-----------------------------------------------------------------------------*/
/** @details
 *  The digital and motor ports can (should) be configured here.
 */
void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
	driveSetup(
		kVexMotor_9,			// drive northeast or front-right motor
		kVexMotor_2,			// drive northwest or front-left motor
		kVexMotor_10,			// drive southeast or back-right motor
		kVexMotor_1				// drive southwest or back-left motor
	);
	// Arm Gearing: https://goo.gl/1UD1ne
	armSetup(
		kVexMotor_8,			// arm top motor pair
		kVexMotor_7,			// arm middle motor pair
		kVexMotor_3,			// arm bottom motor pair
		kVexAnalog_1,			// arm potentiometer
		FALSE,					// reversed potentiometer (values decrease with positive motor speed)
		(1.0 / 7.0),			// gear ratio (1:7 or ~857 ticks per rotation)
		4090,					// resting potentiometer value
		1940					// resting potentiometer value (inverted)
	);
	// Wrist Gearing: https://goo.gl/3fK0Mk
	wristSetup(
		kVexMotor_6,			// wrist motor
		kVexAnalog_2,			// wrist potentiometer
		TRUE,					// reversed potentiometer (values decrease with positive motor speed)
		(1.0 / 3.0),			// gear ratio (1:3 or ~2000 ticks per revolution)
		3170,					// resting potentiometer value
		620						// resting potentiometer value (inverted)
	);
	// Claw Gearing: https://goo.gl/g99rX1
	clawSetup(
		kVexMotor_4,			// claw motor
		kVexAnalog_3,			// claw potentiometer
		TRUE,					// reversed potentiometer (values decrease with positive motor speed)
		(1.0 / 7.0),			// gear ratio (1:7 or ~857 ticks per rotation)
		2540,					// grab potentiometer value
		1800					// open potentiometer value
	);
}

/*-----------------------------------------------------------------------------*/
/** @brief      User initialize                                                */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This function is called after all setup is complete and communication has
 *  been established with the master processor.
 *  Start other tasks and initialize user variables here
 */
void
vexUserInit()
{
	SmartMotorsInit();
	// SmartMotorPtcMonitorEnable();
	SmartMotorCurrentMonitorEnable();
	// TODO: figure out the other two motors plugged into the extender
	SmartMotorsAddPowerExtender(kVexMotor_2, kVexMotor_9);
	armInit();
	wristInit();
	clawInit();
	SmartMotorRun();
}

/*-----------------------------------------------------------------------------*/
/** @brief      Autonomous                                                     */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This thread is started when the autonomous period is started
 */
msg_t
vexAutonomous( void *arg )
{
	(void)arg;

	// Must call this
	vexTaskRegister("auton");

	while (1) {
		// Don't hog cpu
		vexSleep( 25 );
	}

	return (msg_t)0;
}

// #define MotorDriveL     kVexMotor_1
// #define MotorDriveR     kVexMotor_10
// #define MotorWheel kVexMotor_2
// #define MotorClaw kVexMotor_3
// #define MotorWrist kVexMotor_6

/*-----------------------------------------------------------------------------*/
/** @brief      Driver control                                                 */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This thread is started when the driver control period is started
 */
msg_t
vexOperator( void *arg )
{
	// int16_t blink = 0;

	(void)arg;

	// Must call this
	vexTaskRegister("operator");

	// driveStart();
	armStart();
	wristStart();
	// clawStart();

	// Run until asked to terminate
	while (!chThdShouldTerminate()) {
		// flash led/digi out
		// vexDigitalPinSet( kVexDigital_1, (blink++ >> 3) & 1);

		// status on LCD of encoder and sonar
		vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "motor speed %3d", vexMotorGet( kVexMotor_6 ) );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "claw pot %4d", vexAdcGet( clawGetPtr()->potentiometer ) );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "wrist pot %4d", vexAdcGet( kVexAnalog_2 ) );

		// Tank drive
		// left drive
		// SetMotor( MotorWheel, vexControllerGet( Ch3 ) );
		// SetMotor( MotorClaw, vexControllerGet( Ch3 ) );
		// SetMotor( MotorWrist, vexControllerGet( Ch3 ) );
		// SetMotor( MotorWheel, 127 );
		// vexMotorSet( MotorWheel, 127 );

		// right drive
		// vexMotorSet( MotorDriveR, vexControllerGet( Ch2 ) );

		// Don't hog cpu
		vexSleep( 25 );
	}

	return (msg_t)0;
}

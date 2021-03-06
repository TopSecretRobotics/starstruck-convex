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
#include <stdio.h>
#include <string.h>

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "smartmotor.h"
#include "drive.h"
#include "arm.h"
#include "claw.h"
#include "lcd.h"
#include "autonomous.h"

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

// Port 1 has no power expander
// port 9 SW
// port 2 NE                                          36 inches tall 30 inches deep 49 inches wide
// Motor configuration
static vexMotorCfg mConfig[kVexMotorNum] = {
	{ kVexMotor_1,		kVexMotor393R,			kVexMotorNormal,		kVexSensorIME,			0 },
	{ kVexMotor_2,		kVexMotor393R,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_3,		kVexMotorUndefined,		kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_4,		kVexMotor393S,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_5,		kVexMotor393T,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_3 },
	{ kVexMotor_6,		kVexMotor393S,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_7,		kVexMotor393T,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_2 },
	{ kVexMotor_8,		kVexMotor393S,			kVexMotorReversed,		kVexSensorIME,			kImeChannel_1 },
	{ kVexMotor_9,		kVexMotor393R,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_10,		kVexMotor393R,			kVexMotorNormal,		kVexSensorIME,			0 }
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
		kVexMotor_2,			// drive northeast or front-right motor
		kVexMotor_9,			// drive northwest or front-left motor
		kVexMotor_10,			// drive southeast or back-right motor
		kVexMotor_1				// drive southwest or back-left motor
	);
	// Arm Gearing: https://goo.gl/1UD1ne
	armSetup(
		kVexMotor_4,			// arm top motor pair
		kVexMotor_6,			// arm middle motor pair
		kVexMotor_8,			// arm bottom motor pair
		kVexAnalog_1,			// arm potentiometer
		TRUE,					// reversed potentiometer (values decrease with positive motor speed)
		(1.0 / 7.0),			// gear ratio (1:7 or ~857 ticks per rotation)
		3750,					// down potentiometer value
		2880,					// bump potentiometer value
		350						// up potentiometer value
	);
	// Claw Gearing: https://goo.gl/g99rX1
	clawSetup(
		kVexMotor_5,			// left claw motor
		kVexMotor_7,			// right claw motor
		kVexAnalog_2,			// claw potentiometer
		TRUE,					// reversed potentiometer (values decrease with positive motor speed)
		(1.0 / 5.0),			// gear ratio (1:7 or ~1200 ticks per rotation)
		1725,					// grab potentiometer value
		2880					// open potentiometer value
	);
	lcdSetup(VEX_LCD_DISPLAY_1);
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
	SmartMotorCurrentMonitorEnable();
	// SmartMotorPtcMonitorEnable();
	SmartMotorSetPowerExpanderStatusPort(kVexAnalog_3);
	SmartMotorsAddPowerExtender(kVexMotor_2, kVexMotor_7, kVexMotor_8, kVexMotor_9);
	armInit();
	clawInit();
	driveInit();
	SmartMotorRun();
	lcdInit();
	lcdStart();
}

// static inline int
// speedCap(int speed)
// {
// 	if (speed > 127)
// 		speed = 127;
// 	else if (speed < -127)
// 		speed = -127;
// 	return speed;
// }

// static void
// driveSet(drive_t *d, int x, int y, int r)
// {
// 	SetMotor( d->northeast, speedCap( y - x - r ) );
// 	SetMotor( d->northwest, speedCap( y + x + r ) );
// 	SetMotor( d->southeast, speedCap( y + x - r ) );
// 	SetMotor( d->southwest, speedCap( y - x + r ) );
// }

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

	armStart();
	clawStart();
	driveStart();

	// // Lock arm in down position
	// armLockDown();

	// // Lock claw to grab position
	// clawLockGrab();

	// Give the system half a second to restart the LCD
	vexSleep( 500 );
	lcdStart();

	while (1) {
		switch (lcdGetMode()) {
			case kLcdMode0:
				autonomousMode0();
				break;
			case kLcdMode1:
				autonomousMode1();
				break;
			case kLcdMode2:
				autonomousMode2();
				break;
			case kLcdMode3:
				autonomousMode3();
				break;
			case kLcdMode4:
				autonomousMode4();
				break;
			case kLcdMode5:
				autonomousMode5();
				break;
			case kLcdMode6:
				autonomousMode6();
				break;
			case kLcdMode7:
				autonomousMode7();
				break;
			case kLcdMode8:
				autonomousMode8();
				break;
			case kLcdMode9:
				autonomousMode9();
				break;
			default:
				vexSleep(25); // wait 25ms before retry
				break;
		}
		break;
	}

	armLockCurrent();
	clawLockCurrent();
	driveLock();

	return (msg_t)0;
}

// #define MotorDriveL kVexMotor_1
// #define MotorDriveR kVexMotor_10
// #define MotorWheel kVexMotor_2
// #define MotorClaw kVexMotor_3
// #define MotorWrist kVexMotor_6

// #define MENU_ARROW_U	"\xC5"
// #define MENU_ARROW_D	"\xC6"
// #define MENU_ARROW_R	"\xC7"-
// #define MENU_ARROW_L	"\xC8"
// #define MENU_TRIANGLE_R	"\xF6"
// #define MENU_TRIANGLE_L	"\xF7"

// static char menuItems[5][13] = {
// 	"Item 1",
// 	"Item 2",
// 	"Item 3",
// 	"Item 4",
// 	"Item 5"
// };

// static int menuIndex = 0;
// static char *menuItem = NULL;
// static char lcdLine2[20];
// static bool_t leftPressed = FALSE;
// static bool_t rightPressed = FALSE;

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

	armStart();
	clawStart();
	driveStart();

	armLockCurrent();
	clawLockCurrent();
	driveLock();

	// Give the system half a second to restart the LCD
	vexSleep( 500 );
	lcdStart();

	// char buf[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	// size_t buflen = strnlen(buf, 100);
	// char *p = buf;

	// int menuIndex = 0;
	// char *menuItem = menuItems[menuIndex];
	// char lcdLine2[20];
	// bool_t leftPressed = FALSE;
	// bool_t rightPressed = FALSE;
	// menuItem = menuItems[menuIndex];

	// int16_t cmd = 0;

	// Run until asked to terminate
	while (!chThdShouldTerminate()) {
		// flash led/digi out
		// vexDigitalPinSet( kVexDigital_1, (blink++ >> 3) & 1);

		// status on LCD of encoder and sonar
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "Top Secret Robot" );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "m4 %3d m6 %3d", vexMotorGet( kVexMotor_4 ), vexMotorGet( kVexMotor_6 ) );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "claw pot %4d", vexAdcGet( clawGetPtr()->potentiometer ) );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "wrist pot %4d", vexAdcGet( kVexAnalog_2 ) );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "%3d %3d %3d", vexMotorGet( kVexMotor_8 ), vexMotorGet( kVexMotor_7 ), vexMotorGet( kVexMotor_3 ));
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xB0\xB1\xB2\xB" );
		// vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_1, "      MENU      " );
		// snprintf(lcdLine2, 20, "%.1s%*s%*s%.1s", MENU_ARROW_L, 7 + strnlen(menuItem, 12) / 2, menuItem, 7 - strnlen(menuItem, 12) / 2, "", MENU_ARROW_R);
		// vexLcdSet( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, lcdLine2 );
		// // vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "%s%*s%*s%s", MENU_ARROW_L, 8 + strnlen(menuItem, 12) / 2, menuItem, 8 - strnlen(menuItem, 12) / 2, "", MENU_ARROW_R );
		// // vexLcdPrintf( VEX_LCD_DISPLAY_1, VEX_LCD_LINE_2, "%s %s %s", MENU_ARROW_L, menuItem, MENU_ARROW_R );

		// if (leftPressed == FALSE && vexLcdButtonGet( VEX_LCD_DISPLAY_1 ) == kLcdButtonLeft) {
		// 	leftPressed = TRUE;
		// } else if (rightPressed == FALSE && vexLcdButtonGet( VEX_LCD_DISPLAY_1 ) == kLcdButtonRight) {
		// 	rightPressed = TRUE;
		// } else if (leftPressed == TRUE && vexLcdButtonGet( VEX_LCD_DISPLAY_1 ) == kLcdButtonNone) {
		// 	leftPressed = FALSE;
		// 	if (menuIndex == 0) {
		// 		menuIndex = 4;
		// 	} else {
		// 		menuIndex--;
		// 	}
		// 	menuItem = menuItems[menuIndex];
		// } else if (rightPressed == TRUE && vexLcdButtonGet( VEX_LCD_DISPLAY_1 ) == kLcdButtonNone) {
		// 	rightPressed = FALSE;
		// 	if (menuIndex == 4) {
		// 		menuIndex = 0;
		// 	} else {
		// 		menuIndex++;
		// 	}
		// 	menuItem = menuItems[menuIndex];
		// }

		// Tank drive
		// left drive
		// SetMotor( MotorWheel, vexControllerGet( Ch3 ) );
		// SetMotor( MotorClaw, vexControllerGet( Ch3 ) );
		// SetMotor( MotorWrist, vexControllerGet( Ch3 ) );
		// SetMotor( MotorWheel, 127 );
		// vexMotorSet( MotorWheel, 127 );

		//claw left
		// cmd = vexControllerGet( Ch4 );
		// SetMotor( kVexMotor_3, cmd );
		// SetMotor( kVexMotor_4, cmd );
		// SetMotor( kVexMotor_6, cmd );
		// SetMotor( kVexMotor_8, vexControllerGet( Ch2 ) );
		// SetMotor( kVexMotor_6, vexControllerGet( Ch2 ) );
		// vexMotorSet( kVexMotor_8, vexControllerGet( Ch2 ) );

		// cmd = vexControllerGet( Ch4 );
		// if (abs(cmd) < 20) {
		// 	cmd = 0;
		// }
		// SetMotor( kVexMotor_5, cmd, FALSE );
		// SetMotor( kVexMotor_7, cmd, FALSE );
		// SetMotor( kVexMotor_1, cmd, FALSE );
		// SetMotor( kVexMotor_4, cmd, TRUE );
		// SetMotor( kVexMotor_6, cmd, TRUE );
		// SetMotor( kVexMotor_8, cmd, TRUE );
		// SetMotor( kVexMotor_1, cmd );
		// SetMotor( kVexMotor_2, cmd );
		// SetMotor( kVexMotor_9, cmd );
		// SetMotor( kVexMotor_10, cmd );
		// vexMotorSet( kVexMotor_1, cmd );
		// vexMotorSet( kVexMotor_3, cmd );
		// vexMotorSet( kVexMotor_9, cmd );
		// vexMotorSet( kVexMotor_10, cmd );

		// Don't hog cpu
		vexSleep( 25 );
	}

	return (msg_t)0;
}

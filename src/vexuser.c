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

// // Motor configuration
// static vexMotorCfg mConfig[kVexMotorNum] = {
// 	{ kVexMotor_1,		kVexMotor393S,			kVexMotorReversed,		kVexSensorIME,			kImeChannel_2 },
// 	{ kVexMotor_2,		kVexMotorUndefined,		kVexMotorNormal,		kVexSensorNone,			0 },
// 	{ kVexMotor_3,		kVexMotor393S,			kVexMotorNormal,		kVexSensorNone,			0 },
// 	{ kVexMotor_4,		kVexMotor393T,			kVexMotorReversed,		kVexSensorNone,			0 },
// 	{ kVexMotor_5,		kVexMotor393T,			kVexMotorReversed,		kVexSensorIME,			kImeChannel_5 },
// 	{ kVexMotor_6,		kVexMotor393T,			kVexMotorNormal,		kVexSensorNone,			0 },
// 	{ kVexMotor_7,		kVexMotor393T,			kVexMotorReversed,		kVexSensorIME,			kImeChannel_4 },
// 	{ kVexMotor_8,		kVexMotor393T,			kVexMotorReversed,		kVexSensorIME,			kImeChannel_3 },
// 	{ kVexMotor_9,		kVexMotor393S,			kVexMotorReversed,		kVexSensorNone,			0 },
// 	{ kVexMotor_10,		kVexMotor393S,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_1 }
// };

// Motor configuration
static vexMotorCfg mConfig[kVexMotorNum] = {
	{ kVexMotor_1,		kVexMotor393S,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_1 },
	{ kVexMotor_2,		kVexMotor393S,			kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_3,		kVexMotorUndefined,		kVexMotorNormal,		kVexSensorNone,			0 },
	{ kVexMotor_4,		kVexMotor393T,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_5,		kVexMotor393T,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_5 },
	{ kVexMotor_6,		kVexMotor393T,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_7,		kVexMotor393T,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_4 },
	{ kVexMotor_8,		kVexMotor393T,			kVexMotorReversed,		kVexSensorIME,			kImeChannel_3 },
	{ kVexMotor_9,		kVexMotor393S,			kVexMotorReversed,		kVexSensorNone,			0 },
	{ kVexMotor_10,		kVexMotor393S,			kVexMotorNormal,		kVexSensorIME,			kImeChannel_2 }
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
		FALSE,					// normal potentiometer (values increase with positive motor speed)
		(1.0 / 5.0),			// gear ratio (1:7 or ~1200 ticks per rotation)
		120,					// down potentiometer value
		260,					// bump potentiometer value
		2125					// up potentiometer value
	);
	// Claw Gearing: https://goo.gl/g99rX1
	clawSetup(
		kVexMotor_5,			// left claw motor
		kVexMotor_7,			// right claw motor
		kVexAnalog_2,			// claw potentiometer
		TRUE,					// reversed potentiometer (values decrease with positive motor speed)
		(1.0 / 5.0),			// gear ratio (1:7 or ~1200 ticks per rotation)
		1920,					// grab potentiometer value
		2900					// open potentiometer value
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

static void autonomousMode0(void);
static void autonomousMode1(void);
static void autonomousMode2(void);
static void autonomousMode3(void);

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
			default:
				vexSleep(25); // wait 25ms before retry
				break;
		}
	}

	return (msg_t)0;
}

static void
autonomousMode0(void)
{
	// // turn left
	// driveMove( -127,    0, TRUE );
	// // turn right
	// driveMove(  127,    0, TRUE );
	// // forward
	// driveMove(    0,  127, TRUE );
	// // backward
	// driveMove(    0, -127, TRUE );

	// // Open claw
	// clawMove(-127, TRUE);
	// // Close claw
	// clawMove(127, TRUE);

	// // raise arm
	// armMove(127, TRUE);
	// // lower arm
	// armMove(-127, TRUE);


	// Left Tile

	// open claw and raise arm
	clawMove(  -127,  TRUE);
	armMove(    127,  TRUE);

	vexSleep( 200 );
	// lower arm
	armMove( -127, TRUE);

	vexSleep( 200 );
	// stop arm
	armMove( 0, TRUE);

	vexSleep( 100 );
	// stop claw
	clawMove( 0, TRUE);

	// drive forward
	driveMove(  0,  127, TRUE );

	vexSleep( 2000 );

	// turn right
	driveMove(  127,    0, TRUE );

	vexSleep( 1000 );
	//close claw and move forward
	clawMove(127, TRUE);
	driveMove(    0,  127, TRUE );

	vexSleep( 1000 );
	// go in 180 degrees
	driveMove(  127,    0, TRUE );

	vexSleep( 500 );
	// back up against fence
	driveMove(    0, -127, TRUE );

	vexSleep( 2000 );
	// stop moving and raise arm
	driveMove( 0,    0, TRUE );

	armMove(127, TRUE);

	vexSleep( 1000 );
	// put arm down	//
	armMove(-127, TRUE);

	vexSleep( 8000 );

	return;
}

static void
autonomousMode1(void)
{
	// (2) Right tile
		// open claw and raise arm
	clawMove(  -127,  TRUE);
	armMove(    127,  TRUE);

	vexSleep( 200 );
	// lower arm
	armMove( -127, TRUE);

	vexSleep( 200 );
	// stop arm
	armMove( 0, TRUE);

	vexSleep( 100 );
	// stop claw
	clawMove( 0, TRUE);

	// drive forward
	driveMove(  0,  127, TRUE );

	vexSleep( 2000 );

	// turn left
	driveMove(  -127,    0, TRUE );

	vexSleep( 1000 );
	//close claw and move forward
	clawMove(127, TRUE);
	driveMove(    0,  127, TRUE );

	vexSleep( 1000 );
	// go in 180 degrees
	driveMove(  127,    0, TRUE );

	vexSleep( 500 );
	// back up against fence
	driveMove(    0, -127, TRUE );

	vexSleep( 2000 );
	// stop moving and raise arm
	driveMove( 0,    0, TRUE );

	armMove(127, TRUE);

	vexSleep( 1000 );
	// put arm down
	armMove(-127, TRUE);

	vexSleep( 8000 );


	return;
}

static void
autonomousMode2(void)
{
		// (3) Left tile
		// open claw and raise arm
	clawMove(  -127,  TRUE);
	armMove(    127,  TRUE);

	vexSleep( 200 );
	// lower arm
	armMove( -127, TRUE);

	vexSleep( 200 );
	// stop arm
	armMove( 0, TRUE);

	vexSleep( 100 );
	// stop claw
	clawMove( 0, TRUE);

	// drive forward
	driveMove(  0,  127, TRUE );

	vexSleep( 2000 );

	// turn right
	driveMove(  127,    0, TRUE );

	vexSleep( 1000 );
	//close claw & move forward
	clawMove(127, TRUE);
	driveMove(    0,  127, TRUE );

	vexSleep( 1000 );
	// go in 180 degrees
	driveMove(  127,    0, TRUE );

	vexSleep( 500 );
	// back up against fence
	driveMove(    0, -127, TRUE );

	vexSleep( 2000 );
	// stop moving & raise arm
	driveMove( 0,    0, TRUE );
	armMove(127, TRUE);

	vexSleep( 1000 );
	// put arm down	& open claw
	clawMove(-127, TRUE);
	armMove(-127, TRUE);
	driveMove( 0,  127, TRUE);

	vexSleep( 1000 );
	// Turn right
	driveMove(-127, 0, TRUE);

	vexSleep( 1000 );
	// drive forward
	driveMove(0, 127, TRUE);

	vexSleep( 2000 );
	// close claw
	clawMove(127, TRUE);

	vexSleep( 500 );
	// left turn
	driveMove(-127, 0, TRUE);

	vexSleep( 1000 );
	// backup & dump
	driveMove(0, -127, TRUE);
	armMove(127, TRUE);

	vexSleep( 1000);
	// stop drive & drop arm
	driveMove(0, 0, TRUE);
	armMove(-127, TRUE);

	vexSleep( 1000 );
	// stop arm
	armMove(0, TRUE);

	vexSleep( 0 );

	return;
}

static void
autonomousMode3(void)
{

	// open claw and raise arm
	clawMove(  -127,  TRUE);
	armMove(    127,  TRUE);

	vexSleep( 200 );
	// lower arm
	armMove( -127, TRUE);

	vexSleep( 200 );
	// stop arm
	armMove( 0, TRUE);

	vexSleep( 100 );
	// stop claw
	clawMove( 0, TRUE);

	// drive forward
	driveMove(  0,  127, TRUE );

	vexSleep( 2000 );

	// turn left
	driveMove(  -127,    0, TRUE );

	vexSleep( 1000 );
	//close claw & move forward
	clawMove(127, TRUE);
	driveMove(    0,  127, TRUE );

	vexSleep( 1000 );
	// go in 180 degrees
	driveMove(  127,    0, TRUE );

	vexSleep( 500 );
	// back up against fence
	driveMove(    0, -127, TRUE );

	vexSleep( 2000 );
	// stop moving & raise arm
	driveMove( 0,    0, TRUE );
	armMove(127, TRUE);

	vexSleep( 1000 );
	// put arm down	& open claw
	clawMove(-127, TRUE);
	armMove(-127, TRUE);
	driveMove( 0,  127, TRUE);

	vexSleep( 1000 );
	// Turn left
	driveMove(127, 0, TRUE);

	vexSleep( 1000 );
	// drive forward
	driveMove(0, 127, TRUE);

	vexSleep( 2000 );
	// close claw
	clawMove(127, TRUE);

	vexSleep( 500 );
	// left right
	driveMove(127, 0, TRUE);

	vexSleep( 1000 );
	// backup & dump
	driveMove(0, -127, TRUE);
	armMove(127, TRUE);

	vexSleep( 1000);
	// stop drive & drop arm
	driveMove(0, 0, TRUE);
	armMove(-127, TRUE);

	vexSleep( 1000 );
	// stop arm
	armMove(0, TRUE);

	vexSleep( 0 );

	return;
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

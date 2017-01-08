/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2013                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     vexmain.c                                                    */
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


#include <string.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "vex.h"

#include "smartmotor.h"
#include "apollo.h"
#include "pidlib.h"

#include "claw.h"
#include "wrist.h"
#include "arm.h"

/*-----------------------------------------------------------------------------*/
/* Command line related.                                                       */
/*-----------------------------------------------------------------------------*/

static void
cmd_apollo( vexStream *chp, int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	apolloInit();

	// run until any key press
	while( sdGetWouldBlock((SerialDriver *)chp) ) {
		apolloUpdate();
	}

	apolloDeinit();
}

static void
cmd_sm(vexStream *chp, int argc, char *argv[])
{
	(void)argv;
	(void)chp;
	(void)argc;

	SmartMotorDebugStatus();
}

static void
vex_pid_debug(pidController *p)
{
	vex_printf("\tEnabled:    %d\r\n", p->enabled);
	vex_printf("\tKp:         %f\r\n", p->Kp);
	vex_printf("\tKi:         %f\r\n", p->Ki);
	vex_printf("\tKd:         %f\r\n", p->Kd);
	vex_printf("\tKbias:      %f\r\n", p->Kbias);
	vex_printf("\tError:      %f\r\n", p->error);
	vex_printf("\tLast Error: %f\r\n", p->last_error);
	vex_printf("\tIntegral:   %f\r\n", p->integral);
	vex_printf("\tLimit (I):  %f\r\n", p->integral_limit);
	vex_printf("\tDerivative: %f\r\n", p->derivative);
	vex_printf("\tThreshold:  %f\r\n", p->error_threshold);
	vex_printf("\tDrive:      %f\r\n", p->drive);
	vex_printf("\tDrive Raw:  %d\r\n", p->drive_raw);
	vex_printf("\tDrive Cmd:  %d\r\n", p->drive_cmd);
	vex_printf("\tReverse:    %d\r\n", p->sensor_reverse);
	vex_printf("\tSensor:     %d\r\n", p->sensor_value);
	vex_printf("\tTarget:     %d\r\n", p->target_value);
	return;
}

static void
cmd_claw(vexStream *chp, int argc, char *argv[])
{
	(void)argv;
	(void)chp;
	(void)argc;

	vex_printf("Claw Lock PID\r\n");
	// vex_pid_debug(clawGetPtr()->lock);

	return;
}

static void
cmd_wrist(vexStream *chp, int argc, char *argv[])
{
	(void)argv;
	(void)chp;
	(void)argc;

	wrist_t *w = wristGetPtr();
	vex_printf("Wrist\r\n");
	vex_printf("\tReversed:      %d\r\n", w->reversed);
	vex_printf("\tGear Ratio:    %f\r\n", w->gearRatio);
	vex_printf("\tRest:          %d\r\n", w->restValue);
	vex_printf("\tRest Inverted: %d\r\n", w->restInvertedValue);
	vex_printf("Wrist Lock PID\r\n");
	vex_pid_debug(w->lock);

	return;
}

static void
cmd_arm(vexStream *chp, int argc, char *argv[])
{
	(void)argv;
	(void)chp;
	(void)argc;

	arm_t *a = armGetPtr();
	vex_printf("Arm\r\n");
	vex_printf("\tReversed:      %d\r\n", a->reversed);
	vex_printf("\tGear Ratio:    %f\r\n", a->gearRatio);
	vex_printf("\tRest:          %d\r\n", a->restValue);
	vex_printf("\tRest Inverted: %d\r\n", a->restInvertedValue);
	vex_printf("Arm Lock PID\r\n");
	vex_pid_debug(a->lock);

	return;
}

#define SHELL_WA_SIZE THD_WA_SIZE(512)

// Shell command
static const ShellCommand commands[] = {
	{"adc",		vexAdcDebug},
	{"spi",		vexSpiDebug},
	{"motor",	vexMotorDebug},
	{"lcd",		vexLcdDebug},
	{"enc",		vexEncoderDebug},
	{"son",		vexSonarDebug},
	{"ime",		vexIMEDebug},
	{"test",	vexTestDebug},
	{"sm",		cmd_sm},
	{"apollo",	cmd_apollo},
	{"claw",	cmd_claw},
	{"wrist",	cmd_wrist},
	{"arm",		cmd_arm},
	{NULL,		NULL}
};

// configuration for the shell
static const ShellConfig shell_cfg1 = {
	(vexStream *)SD_CONSOLE,
	commands
};

/*-----------------------------------------------------------------------------*/
//  Application entry point.                             */
/*-----------------------------------------------------------------------------*/

int main(void)
{
	Thread *shelltp = NULL;
	short timeout = 0;

	// System initializations.
	// - HAL initialization, this also initializes the configured device drivers
	//   and performs the board-specific initializations.
	// - Kernel initialization, the main() function becomes a thread and the
	//   RTOS is active.
	halInit();
	chSysInit();

	// Set the Team Name
	vexSpiTeamnameSet("TopSecret");

	// Init the serial port associated with the console
	vexConsoleInit();

	// Init VEX
	vexCortexInit();

	// wait for good spi comms
	while( vexSpiGetOnlineStatus() == 0 ) {
		// wait for a while
		chThdSleepMilliseconds(100);
		// dump after 5 seconds
		if (timeout++ == 50) {
			break;
		}
	}

	// Shell manager initialization.
	shellInit();

	// Spin in loop monitoring the shell
	while (TRUE) {
		if (!shelltp) {
			shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
		} else if (chThdTerminated(shelltp)) {
			chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
			shelltp = NULL;           /* Triggers spawning of a new shell.        */
		}

		chThdSleepMilliseconds(50);
	}
}
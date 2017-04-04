/*-----------------------------------------------------------------------------*/
/** @file    autonomous.c                                                      */
/** @brief   The autonomous routines for the robot                             */
/*-----------------------------------------------------------------------------*/

#include "autonomous.h"

#include <math.h>
#include <stdlib.h>

/* Timer Helpers */
typedef struct autotimer_s autotimer_t;

struct autotimer_s {
	unsigned long actual;
	unsigned long target;
};

static autotimer_t autotimer = {
	.actual = 0,
	.target = 0
};

static void
timerStart(unsigned long target)
{
	autotimer.actual = chTimeNow();
	autotimer.target = autotimer.actual + target;
}

static bool
timerActive(void)
{
	return (autotimer.actual < autotimer.target);
}

static void
timerUpdate(void)
{
	vexSleep(25);
	autotimer.actual = chTimeNow();
}

#define timerRun(TARGET, CODE)	\
	do {	\
		timerStart(TARGET);	\
		while (timerActive()) {	\
			CODE	\
			timerUpdate();	\
		}	\
	} while (0)

/* Reusable Routines */
static void
stopMovement(unsigned long target)
{
	timerRun(target, {
		armMove(0, TRUE);
		clawMove(0, TRUE);
		driveMove(0, 0, TRUE);
	});
}

static void
stopDriveMovement(unsigned long target)
{
	timerRun(target, {
		driveMove(0, 0, TRUE);
	});
}

static void
raiseArm(int speed)
{
	armMove(speed, TRUE);
}

static void
lowerArm(int speed)
{
	armMove(-speed, TRUE);
}

static void
driveForward(int speed)
{
	driveMove(0, speed, TRUE);
}

static void
driveBackward(int speed)
{
	driveMove(0, -speed, TRUE);
}

static void
driveRight(int speed)
{
	driveMove(speed, 0, TRUE);
}

static void
driveLeft(int speed)
{
	driveMove(-speed, 0, TRUE);
}

static void
grabClaw(int speed)
{
	clawMove(speed, TRUE);
}

static void
openClaw(int speed)
{
	clawMove(-speed, TRUE);
}


void
autonomousMode0(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();



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

	// move arm down
	armMove( -127, TRUE);
	driveMove(0, 0, TRUE);

	vexSleep(400);
	// raise arm
	armMove(127, TRUE);

	vexSleep(200);
	// lower arm
	armMove(-127, TRUE);
	driveMove(0,0, TRUE);

	vexSleep(500);
	// hold arm down & drive forward & open claw
	clawMove(-100, TRUE);
	armMove(-10, TRUE);
	driveMove(0, 127, TRUE);

	vexSleep(700);

	driveMove(0, 127, TRUE);
	clawMove(0, TRUE);

	vexSleep(800);
	// hold arm & stop drive & close claw
	driveMove( 0, 0, TRUE);
	clawMove(127, TRUE);

	vexSleep(500);
	// hold claw & bump arm
	clawMove(20, TRUE);
	armMove(50, TRUE);

	vexSleep(500);
	// stop arm & drive forward
	armMove(  0, TRUE);
	driveMove(0, 127, TRUE);

	vexSleep(700);
	// turn
	driveMove(127, 0, TRUE);

	vexSleep(600);
	// turn
	driveMove(127, 0, TRUE);

	vexSleep(500);
	// backup to fence
	driveMove(0, -127, TRUE);

	vexSleep(800);
	// stop drive & raise arm & open claw
	driveMove(0, 0, TRUE);
	armMove(60, TRUE);
	clawMove(-20, TRUE);

	vexSleep(800);
	// raise arm
	armMove(127, TRUE);

	vexSleep(1300);

	armMove(127, TRUE);
	// stop claw & lower arm
	clawMove(0, TRUE);
	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);
	// hold arm down & drive forward
	armMove(-10, TRUE);
	driveMove(0, 127, TRUE);

	vexSleep(1000);
	// drive Forward
	driveMove(0, 127, TRUE);

	vexSleep(800);
	// drive forward
	driveMove(0, 127, TRUE);

	vexSleep(1000);

	driveMove( 0, 0, TRUE);

	return;
}

void
autonomousMode1(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	// open claw & move arm down
	clawMove(-127, TRUE);
	armMove( -127, TRUE);
	driveMove(0, 0, TRUE);

	vexSleep(400);
	// raise arm
	armMove(127, TRUE);

	vexSleep(200);
	// lower arm & stop claw
	armMove(-127, TRUE);
	clawMove(  0, TRUE);
	driveMove(0,0, TRUE);

	vexSleep(500);
	// hold arm down & drive forward
	armMove(-10, TRUE);
	driveMove(0, 127, TRUE);

	vexSleep(700);

	driveMove(0, 127, TRUE);

	vexSleep(800);
	// hold arm & stop drive & close claw
	driveMove( 0, 0, TRUE);
	clawMove(127, TRUE);

	vexSleep(500);
	// hold claw & bump arm
	clawMove(20, TRUE);
	armMove(50, TRUE);

	vexSleep(500);
	// stop arm & drive forward
	armMove(  0, TRUE);
	driveMove(0, 127, TRUE);

	vexSleep(700);
	// turn
	driveMove(-127, 0, TRUE);

	vexSleep(600);
	// turn
	driveMove(-127, 0, TRUE);

	vexSleep(500);
	// backup to fence
	driveMove(0, -127, TRUE);

	vexSleep(800);
	// stop drive & raise arm & open claw
	driveMove(0, 0, TRUE);
	armMove(60, TRUE);
	clawMove(-20, TRUE);

	vexSleep(800);
	// raise arm
	armMove(127, TRUE);

	vexSleep(1300);

	armMove(127, TRUE);
	// stop claw & lower arm
	clawMove(0, TRUE);
	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);

	armMove(-127, TRUE);

	vexSleep(500);
	// hold arm down & drive forward
	armMove(-10, TRUE);
	driveMove(0, 127, TRUE);

	vexSleep(1000);
	// drive Forward
	driveMove(0, 127, TRUE);

	vexSleep(800);
	// drive forward
	driveMove(0, 127, TRUE);

	vexSleep(1000);

	driveMove( 0, 0, TRUE);
	return;
}

void
autonomousMode2(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();


	vexSleep(10000);

	return;
}

void
autonomousMode3(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

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

void
autonomousMode4(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	// lift arm & open claw
	armMove(  127, TRUE);
	clawMove(-127, TRUE);

	vexSleep( 500 );
	// drive forward & stop: claw, arm
	driveMove(0, 127,TRUE);
	armMove(  0,     TRUE);
	clawMove( 0,     TRUE);

	vexSleep( 2000 );
	// close claw & turn right
	clawMove( 127, TRUE);
	driveMove(127, 0, TRUE);

	vexSleep(1500);
	// backup
	driveMove(0, -127, TRUE);

	vexSleep(2000);
	// raise arm & stop drive
	driveMove(0, 0, TRUE);
	armMove(127, TRUE);

	vexSleep(1500);
	// open claw
	clawMove(-127, TRUE);

	vexSleep(7500);

	return;
}

void
autonomousMode5(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	// lift arm & open claw
	armMove(  127, TRUE);
	clawMove(-127, TRUE);

	vexSleep( 500 );
	// drive forward & stop: claw, arm
	driveMove(0, 127,TRUE);
	armMove(  0,     TRUE);
	clawMove( 0,     TRUE);

	vexSleep( 2000 );
	// close claw & turn left
	clawMove( 127, TRUE);
	driveMove(-127, 0, TRUE);

	vexSleep(1500);
	// backup
	driveMove(0, -127, TRUE);

	vexSleep(2000);
	// raise arm & stop drive
	driveMove(0, 0, TRUE);
	armMove(127, TRUE);

	vexSleep(1500);
	// open claw
	clawMove(-127, TRUE);

	vexSleep(7500);

	return;
}

void
autonomousMode6(void)
{
	return;
}

void
autonomousMode7(void)
{
	return;
}

void
autonomousMode8(void)
{
	return;
}

void
autonomousMode9(void)
{

	armUnlock();
	clawUnlock();
	driveUnlock();

		// unfold
	{
		timerRun(200, {
			lowerArm(127);
		});
		stopMovement(50);

		timerRun(200, {
			raiseArm(127);
		});
		stopMovement(50);

		timerRun(600, {
			lowerArm(127);
		});

		stopMovement(50);
		armMove(-10, TRUE);
	}

	// drive forward and grab cube
	{
		armUnlock();

		timerRun(1000, {
			lowerArm(127);
			openClaw(127);
		});
		clawLockOpen();

		timerRun(700, {
			driveForward(127);
		});

		clawLockGrab();

		stopDriveMovement(100);
	}

	// lift halfway, drive forward, and turn right
	{

		timerRun(300, {
			raiseArm(60);
		});

		stopMovement(50);
		raiseArm(10);

		timerRun(200, {

			driveForward(127);

		});

		stopDriveMovement(50);

		timerRun(500, {
			driveRight(127);
		});

		stopDriveMovement(50);
		stopMovement(50);
	}

	// backup and dump

	timerRun(1000, {
		driveBackward(127);
	});

	stopDriveMovement(50);

	timerRun(500, {
		raiseArm(127);
		clawLockOpen();
	});

	stopMovement(50);

	timerRun(600, {
		lowerArm(127);
	});

	stopMovement(50);


	vexSleep(5000);

	stopMovement(100);

	return;
}

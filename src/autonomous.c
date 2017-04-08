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

// Grab the Cube, Dump, Grab Three Stars, Dump, Go into Forward & Backward Dump loop.

{
	armUnlock();
	clawUnlock();
	driveUnlock();

	clawUnlock();
	stopMovement(50);
	stopDriveMovement(50);

	grabClaw(10);
	openClaw(0);


		// unfold
	{
		timerRun(200, {
			lowerArm(127);
			openClaw(0);
		});
		stopMovement(50);

		timerRun(200, {
			raiseArm(127);
			openClaw(0);
		});
		stopMovement(50);

		timerRun(600, {
			lowerArm(127);
			openClaw(0);
		});
		timerRun(250, {
			lowerArm(127);
			armLockDown();
		});

		stopMovement(50);
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

		timerRun(400, {
			clawLockGrab();
		});

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

		timerRun(400, {
			driveRight(127);
		});

		stopDriveMovement(50);
		stopMovement(50);
	}
		// backup and dump
	{
		timerRun(800, {
			driveBackward(127);
		});

		stopDriveMovement(50);

		timerRun(900, {
			raiseArm(127);
			clawLockOpen();
		});

		stopMovement(50);
	}
		// lower arm & drive foreward & grab
	{

		timerRun(1000, {
			lowerArm(127);
		});

		timerRun(250, {
			lowerArm(127);
			armLockDown();
		});

		stopMovement(50);

		timerRun(800, {
			driveForward(127);
		});

		stopDriveMovement(50);
		stopMovement(50);

		timerRun(400, {
			openClaw(127);
		});

		timerRun(1000, {
			grabClaw(127);
			clawLockGrab();
		});

		armUnlock();

	}
		// Bump Up, Backup & dump
	{

		timerRun(200, {
			raiseArm(127);
		});


		timerRun(1100, {
			driveBackward(127);
		});

			timerRun(800, {
			raiseArm(127);
		});

		timerRun(100,{
			clawLockOpen();
		});

		timerRun(800, {
			raiseArm(127);
		});

	}
		// Drive Forward, Turn Right, Lower arm
	{

		timerRun(200, {
			driveForward(127);
		});

		timerRun(1200, {
			lowerArm(127);
		});

		timerRun(200, {
			driveRight(127);
		});

		stopDriveMovement(50);

	}
		// Enter loop 3 Times

		// Time One ~ Drive Forward, Grab & Dump
	{
		timerRun(500,{
			openClaw(127);
		});

		timerRun(1500, {
			driveForward(127);
		});

		timerRun(1000, {
			grabClaw(127);
			clawLockGrab();
		});

		timerRun(200, {
			raiseArm(127);
		});

		timerRun(1500, {
			driveBackward(127);
		});

		timerRun(50, {
			driveLeft(127);
		});

		timerRun(600, {
			raiseArm(127);
		});

			timerRun(200, {
				openClaw(127);
			});
		clawUnlock();

		timerRun(600, {
			raiseArm(127);
		});
	}
		// Time Two ~ Drive Forward, Grab & Dump
	{
		timerRun(1200, {
			lowerArm(127);
		});

		timerRun(500,{
			openClaw(127);
		});

		timerRun(1300, {
			driveForward(127);
		});

		timerRun(1500, {
			grabClaw(127);
			clawLockGrab();
		});

		timerRun(200, {
			raiseArm(127);
		});

		timerRun(1500, {
			driveBackward(127);
		});

		timerRun(600, {
			raiseArm(127);
		});

		timerRun(200, {
			openClaw(127);
		});

		clawUnlock();

		timerRun(600, {
			raiseArm(127);
		});
	}
		// Time Three ~ Drive Forward, Grab & Dump
	{
		timerRun(1200, {
			lowerArm(127);
		});

		timerRun(500,{
			openClaw(127);
		});

		timerRun(1300, {
			driveForward(127);
		});

		timerRun(1600, {
			grabClaw(127);
			clawLockGrab();
		});

		timerRun(200, {
			raiseArm(127);
		});

		timerRun(1500, {
			driveBackward(127);
		});

		timerRun(600, {
			raiseArm(127);
		});

		timerRun(200, {
			openClaw(127);
		});

		clawUnlock();

		timerRun(600, {
			raiseArm(127);
		});
	}

	return;
}
void
autonomousMode1(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	return;
}

void
autonomousMode2(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	return;
}

void
autonomousMode3(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	return;
}

void
autonomousMode4(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	return;
}

void
autonomousMode5(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	return;
}

void
autonomousMode6(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

	return;
}

void
autonomousMode7(void)
{
	armUnlock();
	clawUnlock();
	driveUnlock();

		// unfold
	{
		stopDriveMovement(50);

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

		timerRun(250, {
			lowerArm(127);
			armLockDown();
		});

		stopDriveMovement(50);
	}
		// open claw, drive forward & close claw
	{
		timerRun(250, {
			openClaw(127);
		});

		timerRun(1500,{
			driveForward(127);
		});

		stopDriveMovement(25);

		timerRun(500, {
			grabClaw(127);
		});
	}
		// raise arm & turn
	{
		armUnlock();

		timerRun(500, {
			raiseArm(60);
		});

		timerRun(500, {
			driveRight(127);
		});

	}
		// backup and dump
	{
		timerRun(1000, {
			driveBackward(127);
		});

		stopDriveMovement(50);

		timerRun(500, {
			raiseArm(127);
		});

		timerRun(250, {
			openClaw(127);
		});
	}
	return;
}

void
autonomousMode8(void)
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
		timerRun(250, {
			lowerArm(127);
			armLockDown();
	});

		stopMovement(50);
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

		timerRun(100, {
		clawLockGrab();
	});

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

		timerRun(400, {
			driveLeft(127);
		});

		stopDriveMovement(50);
		stopMovement(50);
	}

	// backup and dump

	timerRun(800, {
		driveBackward(127);
	});

	stopDriveMovement(50);

	timerRun(900, {
		raiseArm(127);
		clawLockOpen();
	});

	stopMovement(50);

	// lower arm & drive foreward & grab
	{

	timerRun(1000, {
		lowerArm(127);
	});

	timerRun(250, {
		lowerArm(127);
		armLockDown();
	});

	stopMovement(50);

	timerRun(800, {
		driveForward(127);
	});

	stopDriveMovement(50);
	stopMovement(50);

	timerRun(400, {
		openClaw(127);
	});

	timerRun(1000, {
		clawLockGrab();
	});

	}

	armUnlock();
	// Backup & dump
	{

	timerRun(1100, {
		driveBackward(127);
	});

		timerRun(800, {
		raiseArm(127);
	});


	timerRun(100,{
		clawLockOpen();
	});


	timerRun(800, {
		raiseArm(127);
	});



	}
	return;
}

void
autonomousMode9(void)
{

	armUnlock();
	clawUnlock();
	driveUnlock();

	clawUnlock();
	stopMovement(50);
	stopDriveMovement(50);

	grabClaw(0);
	openClaw(0);


		// unfold
	{
		timerRun(200, {
			lowerArm(127);
			openClaw(0);
		});
		stopMovement(50);

		timerRun(200, {
			raiseArm(127);
			openClaw(0);
		});
		stopMovement(50);

		timerRun(600, {
			lowerArm(127);
			openClaw(0);
		});
		timerRun(250, {
			lowerArm(127);
			armLockDown();
	});

		stopMovement(50);
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

		timerRun(100, {
		clawLockGrab();
	});

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

		timerRun(400, {
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

	timerRun(900, {
		raiseArm(127);
		clawLockOpen();
	});

	stopMovement(50);

	// lower arm & drive foreward & grab
	{

	timerRun(1000, {
		lowerArm(127);
	});

	timerRun(250, {
		lowerArm(127);
		armLockDown();
	});

	stopMovement(50);

	timerRun(800, {
		driveForward(127);
	});

	stopDriveMovement(50);
	stopMovement(50);

	timerRun(400, {
		openClaw(127);
	});

	timerRun(1000, {
		grabClaw(127);
		clawLockGrab();
	});

	}

	armUnlock();
	// bump up Backup & dump
	{

		timerRun(200, {
			raiseArm(127);
		});


	timerRun(1100, {
		driveBackward(127);
	});

		timerRun(800, {
		raiseArm(127);
	});


	timerRun(100,{
		clawLockOpen();
	});


	timerRun(800, {
		raiseArm(127);
	});

	}
	return;
}
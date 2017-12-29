/*
 *intake.h
 */

#ifndef INTAKE_H_

#define INTAKE_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	intakePositionUnknown = -1,
	intakePositionDown = 0,
	intakePositionBump,
	intakePositionUp
} intakePosition_t;

typedef struct intake_s {
	tVexMotor		    intakeMotor;
	tVexAnalogPin	    potentiometer;
	bool_t			    reversed;
	float			    gearRatio;
	int16_t			    downValue;
	int16_t			    bumpValue;
	int16_t			    upValue;
	intakePosition_t	position;
	bool_t			    locked;
	pidController	    *lock;
} intake_t;

extern intake_t	*intakeGetPtr(void);
extern void		intakeSetup(tVexMotor intakeMotor,
						 tVexAnalogPin potentiometer, bool_t reversed, float gearRatio,
						 int16_t downValue, int16_t bumpValue, int16_t upValue);
extern void		intakeInit(void);
extern void		intakeStart(void);
extern void 	intakeMove(int16_t cmd, bool_t immediate);
extern void		intakeLock(void);
extern void		intakeUnlock(void);
extern void		intakeLockDown(void);
extern void		intakeLockBump(void);
extern void		intakeLockUp(void);
extern void		intakeLockCurrent(void);
#ifdef __cplusplus
}
#endif

#endif
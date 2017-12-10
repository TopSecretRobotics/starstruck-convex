/*
 * lift.h
 */

#ifndef LIFT_H_

#define LIFT_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	liftPositionUnknown = -1,
	liftPositionDown = 0,
	liftPositionBump,
	liftPositionUp
} liftPosition_t;

typedef struct lift_s {
	tVexMotor		motor0;
	tVexMotor		motor1;
	tVexMotor		motor2;
	tVexAnalogPin	potentiometer;
	bool_t			reversed;
	float			gearRatio;
	int16_t			downValue;
	int16_t			bumpValue;
	int16_t			upValue;
	liftPosition_t	position;
	bool_t			locked;
	pidController	*lock;
} lift_t;

extern lift_t	*liftGetPtr(void);
extern void		liftSetup(tVexMotor motor0, tVexMotor motor1, tVexMotor motor2,
						 tVexAnalogPin potentiometer, bool_t reversed, float gearRatio,
						 int16_t downValue, int16_t bumpValue, int16_t upValue);
extern void		liftInit(void);
extern void		liftStart(void);
extern void 	liftMove(int16_t cmd, bool_t immediate);
extern void		liftLock(void);
extern void		liftUnlock(void);
extern void		liftLockDown(void);
extern void		liftLockBump(void);
extern void		liftLockUp(void);
extern void		liftLockCurrent(void);
#ifdef __cplusplus
}
#endif

#endif

/*
* when you make arm go through and look for everything with lift.
* you will have to put #include arm into everything with #include lift
*/
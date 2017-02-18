/*
 * arm.h
 */

#ifndef ARM_H_

#define ARM_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	armPositionUnknown = -1,
	armPositionDown = 0,
	armPositionBump,
	armPositionUp
} armPosition_t;

typedef struct arm_s {
	tVexMotor		motor0;
	tVexMotor		motor1;
	tVexMotor		motor2;
	tVexAnalogPin	potentiometer;
	bool_t			reversed;
	float			gearRatio;
	int16_t			downValue;
	int16_t			bumpValue;
	int16_t			upValue;
	armPosition_t	position;
	bool_t			locked;
	pidController	*lock;
} arm_t;

extern arm_t	*armGetPtr(void);
extern void		armSetup(tVexMotor motor0, tVexMotor motor1, tVexMotor motor2,
						 tVexAnalogPin potentiometer, bool_t reversed, float gearRatio,
						 int16_t downValue, int16_t bumpValue, int16_t upValue);
extern void		armInit(void);
extern void		armStart(void);
extern void 	armMove(int16_t cmd, bool_t immediate);
extern void		armLock(void);
extern void		armUnlock(void);
extern void		armLockDown(void);
extern void		armLockBump(void);
extern void		armLockUp(void);
extern void		armLockCurrent(void);
#ifdef __cplusplus
}
#endif

#endif

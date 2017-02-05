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
	pidController	*lock;
	bool_t			isDown;
} arm_t;

extern arm_t	*armGetPtr(void);
extern void		armSetup(tVexMotor motor0, tVexMotor motor1, tVexMotor motor2,
						 tVexAnalogPin potentiometer, bool_t reversed, float gearRatio,
						 int16_t downValue, int16_t bumpValue, int16_t upValue);
extern void		armInit(void);
extern void		armStart(void);

#ifdef __cplusplus
}
#endif

#endif

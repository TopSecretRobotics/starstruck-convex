/*
 * wrist.h
 */

#ifndef WRIST_H_

#define WRIST_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"
#include "autotune.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wrist_s {
	tVexMotor		motor;
	tVexAnalogPin	potentiometer;
	bool_t			reversed;
	float			gearRatio;
	int16_t			restValue;
	int16_t			restInvertedValue;
	pidController	*lock;
	// autotuner_t		at;
} wrist_t;

extern wrist_t	*wristGetPtr(void);
extern void		wristSetup(tVexMotor motor, tVexAnalogPin potentiometer, bool_t reversed, float gearRatio, int16_t restValue, int16_t restInvertedValue);
extern void		wristInit(void);
extern void		wristStart(void);

#ifdef __cplusplus
}
#endif

#endif

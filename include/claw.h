/*
 * claw.h
 */

#ifndef CLAW_H_

#define CLAW_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct claw_s {
	tVexMotor		leftMotor;
	tVexMotor		rightMotor;
	tVexAnalogPin	potentiometer;
	bool_t			sensorReversed;
	float			gearRatio;
	int16_t			grabValue;
	int16_t			openValue;
	pidController	*leftLock;
	pidController	*rightLock;
	bool_t			isGrabbing;
} claw_t;

extern claw_t	*clawGetPtr(void);
extern void		clawSetup(tVexMotor leftMotor, tVexMotor rightMotor, tVexAnalogPin potentiometer,
						  bool_t sensorReversed, float gearRatio, int16_t grabValue, int16_t openValue);
extern void		clawInit(void);
extern void		clawStart(void);
extern void		clawMove(int16_t cmd, bool_t immediate);
#ifdef __cplusplus
}
#endif

#endif

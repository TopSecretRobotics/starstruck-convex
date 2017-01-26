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
	tVexAnalogPin	leftPotentiometer;
	bool_t			leftSensorReversed;
	tVexMotor		rightMotor;
	tVexAnalogPin	rightPotentiometer;
	bool_t			rightSensorReversed;
	float			gearRatio;
	int16_t			leftGrabValue;
	int16_t			leftOpenValue;
	int16_t			rightGrabValue;
	int16_t			rightOpenValue;
	pidController	*leftLock;
	pidController	*rightLock;
	bool_t			isGrabbing;
} claw_t;

extern claw_t	*clawGetPtr(void);
extern void		clawSetup(tVexMotor leftMotor, tVexAnalogPin leftPotentiometer, bool_t leftSensorReversed,
						  tVexMotor rightMotor, tVexAnalogPin rightPotentiometer, bool_t rightSensorReversed,
						  float gearRatio, int16_t leftGrabValue, int16_t leftOpenValue,
						  int16_t rightGrabValue, int16_t rightOpenValue);
extern void		clawInit(void);
extern void		clawStart(void);

#ifdef __cplusplus
}
#endif

#endif

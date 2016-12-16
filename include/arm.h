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
	tVexMotor		topMotorPair;
	tVexMotor		middleMotorPair;
	tVexMotor		bottomMotorPair;
	tVexAnalogPin	potentiometer;
	bool_t			reversed;
	float			gearRatio;
	int16_t			restValue;
	int16_t			restInvertedValue;
	pidController	*lock;
} arm_t;

extern arm_t	*armGetPtr(void);
extern void		armSetup(tVexMotor topMotorPair, tVexMotor middleMotorPair, tVexMotor bottomMotorPair, tVexAnalogPin potentiometer, bool_t reversed, float gearRatio, int16_t restValue, int16_t restInvertedValue);
extern void		armInit(void);
extern void		armStart(void);

#ifdef __cplusplus
}
#endif

#endif

/*
 * setter.h
 */

#ifndef SETTER_H_

#define SETTER_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct setter_s {
	tVexMotor		leftMotor;
	tVexMotor		rightMotor;
	tVexAnalogPin	potentiometer;
	bool_t			sensorReversed;
	float			gearRatio;
	int16_t			grabValue;
	int16_t			openValue;
	bool_t			locked;
	pidController	*leftLock;
	pidController	*rightLock;
	bool_t			isGrabbing;
} setter_t;

extern setter_t	*setterGetPtr(void);
extern void		setterSetup(tVexMotor leftMotor, tVexMotor rightMotor, tVexAnalogPin potentiometer,
						  bool_t sensorReversed, float gearRatio, int16_t grabValue, int16_t openValue);
extern void		setterInit(void);
extern void		setterStart(void);
extern void		setterMove(int16_t cmd, bool_t immediate);
extern void		setterLock(void);
extern void		setterUnlock(void);
extern void		setterLockGrab(void);
extern void		setterLockOpen(void);
extern void		setterLockCurrent(void);
#ifdef __cplusplus
}
#endif

#endif

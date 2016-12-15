/*
 * drive.h
 */

#ifndef DRIVE_H_

#define DRIVE_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "smartmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct drive_s {
	tVexMotor northeast;
	tVexMotor northwest;
	tVexMotor southeast;
	tVexMotor southwest;
} drive_t;

extern drive_t	*driveGetPtr(void);
extern void		driveSetup(tVexMotor northeast, tVexMotor northwest, tVexMotor southeast, tVexMotor southwest);
extern void		driveStart(void);

#ifdef __cplusplus
}
#endif

#endif

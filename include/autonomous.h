/*
 * autonomous.h
 */

#ifndef AUTONOMOUS_H_

#define AUTONOMOUS_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include "pidlib.h"
#include "smartmotor.h"
#include "drive.h"
#include "arm.h"
#include "claw.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void autonomousMode0(void);
extern void autonomousMode1(void);
extern void autonomousMode2(void);
extern void autonomousMode3(void);
extern void autonomousMode4(void);
extern void autonomousMode5(void);
extern void autonomousMode6(void);
extern void autonomousMode7(void);
extern void autonomousMode8(void);
extern void autonomousMode9(void);

#ifdef __cplusplus
}
#endif

#endif

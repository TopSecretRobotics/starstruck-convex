/*
 * autotune.h
 */

#ifndef AUTOTUNE_H_

#define AUTOTUNE_H_

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#ifdef __cplusplus
extern "C" {
#endif

typedef struct autotuner_s {
	bool_t			reversed;
	bool_t			isMax;
	bool_t			isMin;
	float			input;
	float			output;
	float			setpoint;
	float			noiseBand;
	int				controlType;
	bool_t			running;
	unsigned long	peak1;
	unsigned long	peak2;
	unsigned long	lastTime;
	int				sampleTime;
	int				nLookBack;
	int				peakType;
	float			lastInputs[101];
	float			peaks[10];
	int				peakCount;
	bool_t			justchanged;
	bool_t			justevaled;
	float			absMax;
	float			absMin;
	float			oStep;
	float			outputStart;
	float			Ku;
	float			Pu;
} autotuner_t;

extern void		autotuneInit(autotuner_t *t, float input, float output, bool_t reversed);
extern void		autotuneCancel(autotuner_t *t);
extern int		autotuneRuntime(autotuner_t *t);
extern float	autotuneGetKp(autotuner_t *t);
extern float	autotuneGetKi(autotuner_t *t);
extern float	autotuneGetKd(autotuner_t *t);
extern void		autotuneSetLookbackSec(autotuner_t *t, int value);
extern int		autotuneGetLookbackSec(autotuner_t *t);

#ifdef __cplusplus
}
#endif

#endif

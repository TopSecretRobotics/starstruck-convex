/*-----------------------------------------------------------------------------*/
/** @file    autotune.c                                                        */
/** @brief   The autotune system for PID                                       */
/*-----------------------------------------------------------------------------*/

#include "autotune.h"

#include <math.h>
#include <stdlib.h>

static void		autotuneFinishUp(autotuner_t *t);

void
autotuneInit(autotuner_t *t, float input, float output, bool_t reversed)
{
	t->input = input;
	t->output = output;
	t->reversed = reversed;
	t->controlType = 0; // default to PI
	t->noiseBand = 0.5;
	t->running = FALSE;
	t->oStep = 30;
	autotuneSetLookbackSec(t, 10);
	t->lastTime = chTimeNow();
}

void
autotuneCancel(autotuner_t *t)
{
	t->running = FALSE;

	return;
}

int
autotuneRuntime(autotuner_t *t)
{
	t->justevaled = FALSE;
	if (t->peakCount > 9 && t->running) {
		t->running = false;
		autotuneFinishUp(t);
		return 1;
	}
	unsigned long now = chTimeNow();

	if (now >= (t->lastTime + t->sampleTime))
		return 0;
	t->lastTime = now;
	float refVal = t->input;
	t->justevaled = TRUE;
	if (!t->running) {
		// initialize working variables the first time around
		t->peakType = 0;
		t->peakCount = 0;
		t->justchanged = FALSE;
		t->absMax = refVal;
		t->absMin = refVal;
		// t->setpoint = refVal;
		t->running = TRUE;
		t->outputStart = t->output;
		t->output = t->outputStart + t->oStep;
	} else {
		if (refVal > t->absMax)
			t->absMax = refVal;
		if (refVal < t->absMin)
			t->absMin = refVal;
	}

	// oscillate the output base on the input's relation to the setpoint
	if (t->reversed) {
		if (refVal > t->setpoint + t->noiseBand)
			t->output = t->outputStart + t->oStep;
		else if (refVal < t->setpoint - t->noiseBand)
			t->output = t->outputStart - t->oStep;
	} else {
		if (refVal > t->setpoint + t->noiseBand)
			t->output = t->outputStart - t->oStep;
		else if (refVal < t->setpoint - t->noiseBand)
			t->output = t->outputStart + t->oStep;
	}

	t->isMax = TRUE;
	t->isMin = TRUE;
	// identify peaks
	int i;
	for (i = t->nLookBack - 1; i >= 0; i--) {
		float val = t->lastInputs[i];
		if (t->isMax)
			t->isMax = refVal > val;
		if (t->isMin)
			t->isMin = refVal < val;
		t->lastInputs[i + 1] = t->lastInputs[i];
	}
	t->lastInputs[0] = refVal;
	if (t->nLookBack < 9) {
		// we don't want to trust the maxes or mins until the inputs array has been filled
		return 0;
	}

	if (t->isMax) {
		if (t->peakType == 0)
			t->peakType = 1;
		if (t->peakType == -1) {
			t->peakType = 1;
			t->justchanged = TRUE;
			t->peak2 = t->peak1;
		}
		t->peak1 = now;
		t->peaks[t->peakCount] = refVal;
	} else if (t->isMin) {
		if (t->peakType == 0)
			t->peakType = -1;
		if (t->peakType == 1) {
			t->peakType = -1;
			t->peakCount++;
			t->justchanged = TRUE;
		}
		if (t->peakCount < 10)
			t->peaks[t->peakCount] = refVal;
	}

	if (t->justchanged && t->peakCount > 2) {
		// we've transitioned.  check if we can autotune based on the last peaks
		float avgSeparation = (abs(t->peaks[t->peakCount - 1] - t->peaks[t->peakCount - 2]) + abs(t->peaks[t->peakCount - 2] - t->peaks[t->peakCount - 3])) / 2;
		if (avgSeparation < 0.05 * (t->absMax - t->absMin)) {
			autotuneFinishUp(t);
			t->running = FALSE;
			return 1;
		}
	}
	t->justchanged = FALSE;
	return 0;
}

static void
autotuneFinishUp(autotuner_t *t)
{
	t->output = t->outputStart;
	// we can generate tuning parameters!
	t->Ku = 4 * (2 * t->oStep) / ((t->absMax - t->absMin) * 3.14159);
	t->Pu = (float)(t->peak1 - t->peak2) / 1000;

	return;
}

float
autotuneGetKp(autotuner_t *t)
{
	return (t->controlType == 1) ? (0.6 * t->Ku) : (0.4 * t->Ku);
}

float
autotuneGetKi(autotuner_t *t)
{
	return (t->controlType == 1) ? (1.2 * t->Ku / t->Pu) : (0.48 * t->Ku / t->Pu); // Ki = Kc/Ti
}

float
autotuneGetKd(autotuner_t *t)
{
	return (t->controlType == 1) ? (0.075 * t->Ku * t->Pu) : 0; // Kd = Kc * Td
}

void
autotuneSetLookbackSec(autotuner_t *t, int value)
{
	if (value < 1)
		value = 1;

	if (value < 25) {
		t->nLookBack = value * 4;
		t->sampleTime = 250;
	} else {
		t->nLookBack = 100;
		t->sampleTime = value * 10;
	}

	return;
}

int
autotuneGetLookbackSec(autotuner_t *t)
{
	return (t->nLookBack * t->sampleTime / 1000);
}
// i don't care enough about this project to add proper licensing so consider
// this public domain

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "lv2/core/lv2.h"

#define URI "https://github.com/mannikim/mkrush"

enum PortIndex {
	PORT_INPUT = 0,
	PORT_OUTPUT = 1,
	PORT_SCALE = 2,
	PORT_HOLD = 3,
	PORT_ENABLE_SCALE = 4,
};

struct Plugin {
	const float *input;
	float *output;

	const float *scale;
	const float *hold;
	const float *enable;

	float last_sample_val;
	uint32_t proc_samples;
};

static LV2_Handle
instantiate(const LV2_Descriptor *descriptor,
	    double rate,
	    const char *bundle_path,
	    const LV2_Feature *const *features)
{
	(void)descriptor;
	(void)rate;
	(void)bundle_path;
	(void)features;

	struct Plugin *self = calloc(1, sizeof(*self));
	return self;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void *data)
{
	if (instance == NULL)
		return;

	struct Plugin *self = instance;

	switch ((enum PortIndex)port)
	{
	case PORT_INPUT:
		self->input = data;
		break;
	case PORT_OUTPUT:
		self->output = data;
		break;
	case PORT_SCALE:
		self->scale = data;
		break;
	case PORT_HOLD:
		self->hold = data;
		break;
	case PORT_ENABLE_SCALE:
		self->enable = data;
	}
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	if (instance == NULL)
		return;

	struct Plugin *self = instance;

	const float *const input = self->input;
	float *const output = self->output;
	float scale = *self->scale;
	float hold = *self->hold;
	float enable = *self->enable;
	uint32_t proc_samples = self->proc_samples;

	for (uint32_t pos = 0; pos < n_samples; pos++)
	{
		if (proc_samples == 0)
		{
			self->last_sample_val = input[pos];
		}

		if (enable > 0)
		{
			output[pos] =
				roundf(self->last_sample_val * scale) / scale;
		} else
		{
			output[pos] = self->last_sample_val;
		}

		proc_samples++;
		if (proc_samples >= (uint32_t)hold)
		{
			proc_samples = 0;
		}
	}

	self->proc_samples = proc_samples;
}

static void
cleanup(LV2_Handle instance)
{
	if (instance == NULL)
		return;

	free(instance);
}

static const LV2_Descriptor descriptor = {
	URI,
	instantiate,
	connect_port,
	NULL,
	run,
	NULL,
	cleanup,
	NULL,
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *
lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}

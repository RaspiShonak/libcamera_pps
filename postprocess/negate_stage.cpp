#include "negate_stage.hpp"

void NegateStage::process(CompletedRequest &completed_request)
{
	int w, h, stride;
	Stream *stream = app_->ViewfinderStream(&w, &h, &stride);

	void *mem = app_->Mmap(completed_request.buffers[stream])[0];

	uint8_t *ptr = (uint8_t *)mem;

	unsigned int num_pixels = stride * h * 3 / 2;
	for (unsigned int i = 0; i < num_pixels; i++)
		*(ptr++) ^= 0xff;
}

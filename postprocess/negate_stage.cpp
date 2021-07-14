#include "postprocess/negate_stage.hpp"

void NegateStage::process(CompletedRequest &completed_request, libcamera::Stream *stream)
{
	int w, h, stride;
	app_->StreamDimensions(stream, &w, &h, &stride);
	libcamera::Span<uint8_t> buffer = app_->Mmap(completed_request.buffers[stream])[0];
	uint8_t *ptr = buffer.data();

	//Everything beyond this point is image processing...
	for (unsigned int i = 0; i < buffer.size(); i++)
		*(ptr++) ^= 0xff;
}

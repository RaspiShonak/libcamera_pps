#include "postprocess/negate_stage.hpp"

void NegateStage::process(CompletedRequest &completed_request)
{
	int w, h, stride;
	std::vector<Stream *> streams;
	streams = app_->GetActiveStreams();

	for (auto const stream : streams)
	{
		app_->StreamDimensions(stream, &w, &h, &stride);
		libcamera::Span<uint8_t> buffer = app_->Mmap(completed_request.buffers[stream])[0];

		uint8_t *ptr = buffer.data();

		for (unsigned int i = 0; i < buffer.size(); i++)
			*(ptr++) ^= 0xff;
	}
}

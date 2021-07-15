#include "postprocess/post_process_stage.hpp"

class NegateStage : public PostProcessStage
{
public:
	NegateStage(LibcameraApp *app) : PostProcessStage(app, { libcamera::formats::YUV420 }) {}

protected:
	void process(CompletedRequest &completed_request, libcamera::Stream *stream);
};

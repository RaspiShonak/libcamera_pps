#include "postprocess/post_process_stage.hpp"

class NegateStage : public PostProcessStage
{
public:
	NegateStage(LibcameraApp *app):PostProcessStage(app)
	{
	}

protected:
	void process(CompletedRequest &completed_request, libcamera::Stream *stream);
};

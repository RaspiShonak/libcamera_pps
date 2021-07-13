#include "postprocess/post_process_stage.hpp"

class SobelCVStage : public PostProcessStage
{
public:
	SobelCVStage(LibcameraApp *app):PostProcessStage(app)
	{
	}

protected:
	void process(CompletedRequest &completed_request, libcamera::Stream *stream);
};

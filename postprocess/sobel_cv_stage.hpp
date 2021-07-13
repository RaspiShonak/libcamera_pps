#include "postprocess/post_process_stage.hpp"

class SobelCVStage : public PostProcessStage
{
public:
	SobelCVStage(LibcameraApp *app, const std::vector<libcamera::PixelFormat> pixel_formats)
		: PostProcessStage(app, pixel_formats)
	{
	}

protected:
	void process(CompletedRequest &completed_request, libcamera::Stream *stream);
};

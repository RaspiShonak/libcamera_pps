#include "post_process_stage.hpp"

class SobelCVStage : public PostProcessStage
{
	using PostProcessStage::PostProcessStage;

protected:
	void process(CompletedRequest &completed_request);
};

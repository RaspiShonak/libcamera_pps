#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "postprocess/post_process_stage.hpp"

using namespace std;
using namespace cv;

class FaceDetectCVStage : public PostProcessStage
{
public:
	FaceDetectCVStage(LibcameraApp *app) : PostProcessStage(app, { libcamera::formats::YUV420 }) {}

protected:
	void process(CompletedRequest &completed_request, libcamera::Stream *stream);

private:
	void detectFeatures(Mat &img, CascadeClassifier &cascade, CascadeClassifier &nestedCascade, double scale);

	void drawFeatures(Mat &img, double scale);
};

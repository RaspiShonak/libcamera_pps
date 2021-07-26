#include <libcamera/stream.h>

#include "../core/libcamera_app.hpp"
#include "../core/post_processing_stage.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

using Stream = libcamera::Stream;

class SobelCVStage : public PostProcessingStage
{
public:
	SobelCVStage(LibcameraApp *app) : PostProcessingStage(app) {}

	char const *Name() const override;

	void Read(boost::property_tree::ptree const &params) override;

	void Configure();

	void Process(CompletedRequest &completed_request);

private:
	Stream *stream_;
	int ksize_ = 3;
};

#define NAME "sobelcv"

char const *SobelCVStage::Name() const
{
	return NAME;
}

void SobelCVStage::Read(boost::property_tree::ptree const &params)
{
	ksize_ = params.get<int16_t>("ksize", 3);
}

void SobelCVStage::Configure()
{
	stream_ = app_->GetActiveStream();
}

void SobelCVStage::Process(CompletedRequest &completed_request)
{
	int w, h, stride;
	app_->StreamDimensions(stream_, &w, &h, &stride);
	libcamera::Span<uint8_t> buffer = app_->Mmap(completed_request.buffers[stream_])[0];
	uint8_t *ptr = (uint8_t *)buffer.data();

	//Everything beyond this point is image processing...

	uint8_t value = 128;
	int num = (stride * h) / 2;
	Mat src = Mat(h, w, CV_8U, ptr, stride);
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	memset(ptr + stride * h, value, num);

	//Check if image is loaded fine
	if (src.empty())
	{
		std::cout << "Error Loading Image" << std::endl;
	}

	// Remove noise by blurring with a Gaussian filter ( kernal size = 3 )
	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	Mat grad_x, grad_y;

	//Scharr(src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT);
	Sobel(src, grad_x, ddepth, 1, 0, ksize_, scale, delta, BORDER_DEFAULT);
	//Scharr(src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT);
	Sobel(src, grad_y, ddepth, 0, 1, ksize_, scale, delta, BORDER_DEFAULT);

	// converting back to CV_8U
	convertScaleAbs(grad_x, grad_x);
	convertScaleAbs(grad_y, grad_y);

	//weight the x and y gradients and add their magnitudes
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, src);
}

static PostProcessingStage *Create(LibcameraApp *app)
{
	return new SobelCVStage(app);
}

static RegisterStage reg(NAME, &Create);

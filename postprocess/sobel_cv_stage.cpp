#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "sobel_cv_stage.hpp"

using namespace cv;

void SobelCVStage::process(CompletedRequest &completed_request)
{
	int w, h, stride;
	Stream *stream = app_->ViewfinderStream(&w, &h, &stride);

	uint8_t value = 128;
	int num = (stride * h) / 2;

	void *mem = app_->Mmap(completed_request.buffers[stream])[0];

	uint8_t *ptr = (uint8_t *)mem;
	Mat src = Mat(h, stride, CV_8U, ptr);
	Mat grad;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16U;

	memset(ptr + stride * h, value, num);

	//Check if image is loaded fine
	if (src.empty())
	{
		std::cout << "Error Loading Image" << std::endl;
	}

	// Remove noise by blurring with a Gaussian filter ( kernal size = 3 )
	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	//Scharr(src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT);
	Sobel(src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	//Scharr(src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT);
	Sobel(src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	// converting back to CV_8U
	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);

	//weight the x and y gradients and add their magnitudes
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	memcpy(ptr, grad.ptr(), h * stride);
}

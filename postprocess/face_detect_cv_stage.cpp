#include <iostream>
#include <libcamera/stream.h>
#include "postprocess/face_detect_cv_stage.hpp"

string cascadeName;
string nestedCascadeName;
vector<Rect> faces, faces2;
vector<Rect> nestedObjects;

void FaceDetectCVStage::process(CompletedRequest &completed_request, libcamera::Stream *stream)
{
	int w, h, stride;
	app_->StreamDimensions(stream, &w, &h, &stride);
	libcamera::Span<uint8_t> buffer = app_->Mmap(completed_request.buffers[stream])[0];
	uint8_t *ptr = (uint8_t *)buffer.data();

	Mat image = Mat(h, w, CV_8U, ptr, stride);
	CascadeClassifier cascade, nestedCascade;
	double scale;
	cascadeName = "/home/pi/opencv-3.4.7/data/haarcascades/haarcascade_frontalface_alt.xml";
	nestedCascadeName = "/home/pi/opencv-3.4.7/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
	scale = 1;

	if (!nestedCascade.load(samples::findFileOrKeep(nestedCascadeName)))
		cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
	if (!cascade.load(samples::findFile(cascadeName)))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
	}

	//Everything beyond this point is image processing...
	drawFeatures(image, scale);

	if (completed_request.sequence % 15 == 0)
	{
		detectFeatures(image, cascade, nestedCascade, scale);
	}
}

void FaceDetectCVStage::detectFeatures(Mat &img, CascadeClassifier &cascade, CascadeClassifier &nestedCascade,
									   double scale)
{
	Mat smallImg;
	double fx = 1 / scale;
	resize(img, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
	equalizeHist(smallImg, smallImg);

	cascade.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
}

void FaceDetectCVStage::drawFeatures(Mat &img, double scale)
{
	const static Scalar colors[] = {
		Scalar(255,0,0), Scalar(255,128,0), Scalar(255,255,0), Scalar(0,255,0),
		Scalar(0,128,255), Scalar(0,255,255), Scalar(0,0,255), Scalar(255,0,255)
	};

	for (size_t i = 0; i < faces.size(); i++)
	{
		Rect r = faces[i];
		Point center;
		Scalar color = colors[i % 8];
		int radius;
		double aspect_ratio = (double)r.width / r.height;

		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			center.x = cvRound((r.x + r.width * 0.5) * scale);
			center.y = cvRound((r.y + r.height * 0.5) * scale);
			radius = cvRound((r.width + r.height) * 0.25 * scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
		else
			rectangle(img, Point(cvRound(r.x * scale), cvRound(r.y * scale)),
					  Point(cvRound((r.x + r.width - 1) * scale), cvRound((r.y + r.height - 1) * scale)), color, 3, 8,
					  0);
	}
}

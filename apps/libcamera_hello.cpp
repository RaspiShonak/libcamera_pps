/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Ltd.
 *
 * libcamera_hello.cpp - libcamera "hello world" app.
 */

#include <chrono>

#include "core/libcamera_app.hpp"
#include "core/options.hpp"
#include "postprocess/face_detect_cv_stage.hpp"
#include "postprocess/negate_stage.hpp"
#include "postprocess/sobel_cv_stage.hpp"

using namespace std::placeholders;

// The main event loop for the application.

static void event_loop(LibcameraApp &app)
{
	Options const *options = app.GetOptions();
	SobelCVStage stage(&app);

	app.OpenCamera();
	app.ConfigureViewfinder();
	app.StartCamera();
	stage.SetCallback(std::bind(&LibcameraApp::ShowPreview, &app, _1, app.ViewfinderStream()));
	// When the preview window is done with a set of buffers, queue them back to libcamera.
	app.SetPreviewDoneCallback(std::bind(&LibcameraApp::QueueRequest, &app, _1));
	auto start_time = std::chrono::high_resolution_clock::now();

	for (unsigned int count = 0; ; count++)
	{
		LibcameraApp::Msg msg = app.Wait();
		if (msg.type == LibcameraApp::MsgType::Quit)
			return;
		else if (msg.type != LibcameraApp::MsgType::RequestComplete)
			throw std::runtime_error("unrecognised message!");

		if (options->verbose)
			std::cout << "Viewfinder frame " << count << std::endl;
		auto now = std::chrono::high_resolution_clock::now();
		if (options->timeout && now - start_time > std::chrono::milliseconds(options->timeout))
			return;

		CompletedRequest &completed_request = std::get<CompletedRequest>(msg.payload);

		stage.PostProcess(completed_request);
		//app.ShowPreview(completed_request, app.ViewfinderStream());
	}
}

int main(int argc, char *argv[])
{
	try
	{
		LibcameraApp app;
		Options *options = app.GetOptions();
		if (options->Parse(argc, argv))
		{
			if (options->verbose)
				options->Print();

			event_loop(app);
		}
	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: *** " << e.what() << " ***" << std::endl;
		return -1;
	}
	return 0;
}

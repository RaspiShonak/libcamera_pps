#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

#include "core/completed_request.hpp"
#include "core/libcamera_app.hpp"

using OutputReadyCallback = std::function<void(CompletedRequest &)>;
using Stream = libcamera::Stream;

class PostProcessStage
{
public:
	PostProcessStage(LibcameraApp *app, const std::vector<libcamera::PixelFormat> pixel_formats);
	virtual ~PostProcessStage();

	void PostProcess(CompletedRequest &request);
	void SetCallback(OutputReadyCallback callback) { output_ready_callback_ = callback; }

private:
	void filterStreams(std::vector<Stream *> &streams);
	void outputThread();

	const std::vector<PostProcessStage *> stages_;
	std::queue<std::future<CompletedRequest>> results_;
	std::thread output_thread_;
	bool quit_;
	OutputReadyCallback output_ready_callback_;
	std::mutex mutex_;
	std::condition_variable cv_;

protected:
	virtual void process(CompletedRequest &request, Stream *stream) = 0;

	LibcameraApp *app_;
	const std::vector<libcamera::PixelFormat> pixel_formats_;
};

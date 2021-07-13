#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "core/completed_request.hpp"
#include "core/libcamera_app.hpp"

using OutputReadyCallback = std::function<void(CompletedRequest &)>;
using Stream = libcamera::Stream;

class PostProcessStage
{
public:
	PostProcessStage(LibcameraApp *app);

	virtual ~PostProcessStage();

	void PostProcess(CompletedRequest &completed_request);

	void SetCallback(OutputReadyCallback callback) { output_ready_callback_ = callback; }

private:
	static const int NUM_PPS_THREADS = 4;

	void postProcessThread(int num);

	void outputThread();

	OutputReadyCallback output_ready_callback_;

	std::queue<CompletedRequest> pps_input_queue_;
	std::mutex pps_input_mutex_;
	std::condition_variable pps_input_cond_var_;
	std::thread pps_input_thread_[NUM_PPS_THREADS];
	std::queue<CompletedRequest> pps_output_queue_[NUM_PPS_THREADS];
	std::mutex pps_output_mutex_;
	std::condition_variable pps_output_cond_var_;
	std::thread pps_output_thread_;
	std::vector<Stream *> streams_;
	bool abort_;
	uint64_t sequence_;

protected:
	virtual void process(CompletedRequest &completed_request, libcamera::Stream *stream);
	LibcameraApp *app_;
};

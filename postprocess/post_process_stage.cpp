#include <chrono>

#include "postprocess/post_process_stage.hpp"

using namespace std::literals::chrono_literals;

PostProcessStage::PostProcessStage(LibcameraApp *app, const std::vector<libcamera::PixelFormat> pixel_formats)
	: app_(app), pixel_formats_(pixel_formats), quit_(false)
{
	output_thread_ = std::thread(&PostProcessStage::outputThread, this);
}

PostProcessStage::~PostProcessStage()
{
	{
		std::unique_lock<std::mutex> l(mutex_);
		quit_ = true;
		cv_.notify_one();
	}

	output_thread_.join();
}

void PostProcessStage::PostProcess(CompletedRequest &request)
{
	std::vector<Stream *> streams = app_->GetActiveStreams();

	filterStreams(streams);

	// Test the case where we have no streams to process and we can call output_ready_callback_ directly.
	// Do we want to perhaps have this run in the output thread context for consistency?
	if (!streams.size())
	{
		output_ready_callback_(request);
		return;
	}

	std::unique_lock<std::mutex> l(mutex_);

	// Run an async task per stream.
	for (auto stream : streams)
	{
		auto process_fn =
			[this, stream](CompletedRequest r) -> auto
			{
				process(r, stream);
				cv_.notify_one();
				return r;
			};

		// Move the CompletedRequest structure to action in the last Future. This tells the output thread that all
		// the streams for this request have been processed. All other Futures are given an empty CompletedRequest
		// structure.
		std::future<CompletedRequest> f =
			std::async(process_fn, std::move(stream == streams.back() ? request : CompletedRequest()));

		results_.push(std::move(f));
	}
}

void PostProcessStage::filterStreams(std::vector<Stream *> &streams)
{
	auto filter_fn =
		[this](Stream *s)
		{
			return std::find(pixel_formats_.begin(), pixel_formats_.end(),
							 s->configuration().pixelFormat) == pixel_formats_.end();
		};

	// Remove any streams that use an unsupported pixel format.
	streams.erase(std::remove_if(streams.begin(), streams.end(), filter_fn), streams.end());
}

void PostProcessStage::outputThread()
{
	while (true)
	{
		CompletedRequest request;

		{
			std::unique_lock<std::mutex> l(mutex_);
			cv_.wait(l, [this] { return !results_.empty() || quit_; });

			if (quit_)
				break;

			// request.buffers.size() != 0 for the last stream in the request.
			while (!request.buffers.size() && !results_.empty() &&
				   results_.front().wait_for(100us) == std::future_status::ready)
			{
				request = results_.front().get();
				results_.pop();
			}
		}

		if (request.buffers.size())
			output_ready_callback_(request);
	}
}

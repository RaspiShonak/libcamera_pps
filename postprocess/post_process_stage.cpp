#include <chrono>
#include <iostream>

#include "postprocess/post_process_stage.hpp"

PostProcessStage::PostProcessStage(LibcameraApp *app) : app_(app), abort_(false), sequence_(0)
{
	pps_output_thread_ = std::thread(&PostProcessStage::outputThread, this);
	for (int i = 0; i < NUM_PPS_THREADS; i++)
		pps_input_thread_[i] = std::thread(std::bind(&PostProcessStage::postProcessThread, this, i));
}

PostProcessStage::~PostProcessStage()
{
	abort_ = true;
	for (int i = 0; i < NUM_PPS_THREADS; i++)
		pps_input_thread_[i].join();
	pps_output_thread_.join();
}

void PostProcessStage::PostProcess(CompletedRequest &completed_request)
{
	std::lock_guard<std::mutex> lock(pps_input_mutex_);
	pps_input_queue_.push(completed_request);
	pps_input_cond_var_.notify_all();
}

void PostProcessStage::postProcessThread(int num)
{
	CompletedRequest completed_request;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(pps_input_mutex_);
			while (true)
			{
				using namespace std::chrono_literals;
				if (abort_)
				{
					return;
				}
				if (!pps_input_queue_.empty())
				{
					completed_request = pps_input_queue_.front();
					pps_input_queue_.pop();
					break;
				}
				else
					pps_input_cond_var_.wait_for(lock, 200ms);
			}
		}

		process(completed_request);

		std::lock_guard<std::mutex> lock(pps_output_mutex_);
		pps_output_queue_[num].push(completed_request);
		pps_output_cond_var_.notify_one();
	}
}

void PostProcessStage::outputThread()
{
	CompletedRequest completed_request;
	int sequence_ = 0;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(pps_output_mutex_);
			while (true)
			{
				using namespace std::chrono_literals;
				if (abort_)
					return;
				// We look for the thread that's completed the frame we want next.
				// If we don't find it, we wait.
				for (auto &q : pps_output_queue_)
				{
					if (!q.empty() && q.front().sequence == sequence_)
					{
						completed_request = q.front();
						q.pop();
						goto got_item;
					}
				}
				pps_output_cond_var_.wait_for(lock, 200ms);
			}
		}
	got_item:
		output_ready_callback_(completed_request);
		sequence_++;
	}
}

void PostProcessStage::process(CompletedRequest &completed_request)
{
}

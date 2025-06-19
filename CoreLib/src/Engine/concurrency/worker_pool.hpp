#pragma once

#include "ts_ring_buffer.hpp"
#include "types.hpp"

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

#include "core.hpp"

namespace rnd
{
	class worker_pool
	{
	public:
		using Task = std::move_only_function<void()>;

		worker_pool(rnd::sz num_threads)
		{
			for (rnd::sz i = 0; i < num_threads; ++i)
			{
				// start up all the threads
				std::thread worker(&worker_pool::run, this);
				worker.detach();
			}
		}

		template <typename Fn, typename... Args>
		auto execute(Fn&& func, Args&& ...args)
		{
			const auto no_param_fn = std::bind(std::forward<Fn>(func), std::forward<Args>(args)...);
			std::packaged_task<std::invoke_result_t<Fn, Args...>()> pack_task{ no_param_fn };
			auto fut = pack_task.get_future();
			_task_buffer.push(std::move(pack_task));
			_submitted_tasks += 1;
			return fut;
		}

		template <typename Fn, typename... Args>
		void dispatch(rnd::u32 num_jobs, rnd::u32 group_size, Fn&& job, Args&&...args)
		{
			ASSERT(num_jobs != 0 && group_size != 0, "number of jobs and groups size must be greater than 0");

			const rnd::u32 group_count = (num_jobs + group_size - 1) / group_size;
			_submitted_tasks += group_count;

			auto bound_job = std::bind(std::forward<Fn>(job), std::placeholders::_1, std::forward<Args>(args)...);

			for (rnd::u32 gi = 0; gi < group_count; ++gi)
			{
				_task_buffer.push([gi, group_size, num_jobs, bound_job]() mutable
					{
						const rnd::u32 start = gi * group_size;
						const rnd::u32 end = std::min(start + group_size, num_jobs);
						for (rnd::u32 i = start; i < end; ++i)
							bound_job(i);
					});
			}
		}

		void wait_for_all_done()
		{
			std::unique_lock lck{ _mtx };
			_all_done.wait(lck, [this] {
				return _submitted_tasks == _finished_tasks;
				});
		}

	private:
		void run()
		{
			for (;;)
			{
				Task t = std::move(_task_buffer.pop());
				t();
				_finished_tasks.fetch_add(1);
				if (_submitted_tasks == _finished_tasks)
					_all_done.notify_all();
			}
		}

	private:
		rnd::ts_ring_buffer<Task, 256> _task_buffer;
		std::mutex _mtx;
		std::condition_variable _all_done;

		rnd::sz _submitted_tasks = 0;
		std::atomic<rnd::sz> _finished_tasks = 0;
	};
}
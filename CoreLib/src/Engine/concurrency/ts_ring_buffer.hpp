#pragma once

#include <semaphore>
#include <mutex>
#include <array>
#include <utility>
#include "types.hpp"

namespace rnd
{
	template <typename T, size_t Size>
	struct ts_ring_buffer
	{
		ts_ring_buffer()
			:
			_free_slots(Size),
			_filled_slots(0)
		{}

		template <typename U>
		void push(U&& item)
		{
			_free_slots.acquire();
			{
				std::lock_guard lock(_mtx);
				_data[_head] = std::forward<U>(item);
				_head = (_head + 1) % Size;
			}
			_filled_slots.release();
		}

		T pop()
		{
			_filled_slots.acquire();
			T item;
			{
				std::lock_guard lock(_mtx);
				item = std::move(_data[_tail]);
				_tail = (_tail + 1) % Size;
			}
			_free_slots.release();
			return item;
		}

		template <typename U>
		bool try_push(U&& item)
		{
			if (!_free_slots.try_acquire())
				return false;
			{
				std::lock_guard lock(_mtx);
				_data[_head] = std::forward<U>(item);
				_head = (_head + 1) % Size;
			}
			_filled_slots.release();
			return true;
		}

		bool try_pop(T& item)
		{
			if (!_filled_slots.try_acquire())
				return false;
			{
				std::lock_guard lock(_mtx);
				item = std::move(_data[_tail]);
				_tail = (_tail + 1) % Size;
			}
			_free_slots.release();
			return true;
		}

	private:
		std::array<T, Size> _data{};
		rnd::sz _head = 0, _tail = 0;

		std::mutex _mtx;
		std::counting_semaphore<Size> _free_slots, _filled_slots;
	};
}
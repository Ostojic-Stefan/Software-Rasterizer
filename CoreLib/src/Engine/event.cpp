#include "pch.h"
#include "event.hpp"

namespace rnd::event
{
	static inline std::vector<Callback> callbacks;

	void registerCallback(Callback cb)
	{
		callbacks.push_back(cb);
	}

	void trigger_event(const Event& e)
	{
		for (const auto& cb : callbacks)
			cb(e);
	}
}



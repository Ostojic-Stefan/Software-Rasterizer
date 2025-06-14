#pragma once

#include "types.hpp"
#include "renderer.hpp"

struct iscene
{
	virtual void update(rnd::f32 dt) = 0;
	virtual void render() = 0;
	virtual ~iscene() = default;
};

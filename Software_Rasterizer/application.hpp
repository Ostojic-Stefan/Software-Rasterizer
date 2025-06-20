#pragma once

#include <Engine/engine.hpp>

#include "movement_camera.hpp"
#include "cube_scene.hpp"
#include "platform.hpp"

#include "the_renderer.hpp"
#include "model_scene.hpp"

class application
{
public:
	application();
	~application();
	void run();
	void update(rnd::f32 dt);
	void render();
private:
private:
	rnd::framebuffer fb;

	rnd::b8 running = true;
	rnd::f32 dt = 0.f;
	rnd::f32 total_time = 0.f;

	// the_renderer<shader_program> renderer;

	cube_plain_scene _cube_scene;
	mode_scene _model_scene;
};

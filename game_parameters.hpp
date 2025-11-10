#pragma once

struct Parameters
{
	static constexpr int game_width = 1920;
	static constexpr int game_height = 1080;
	// gravitational pull
	static constexpr float g = -9.8f;

	static constexpr float physics_scale = 30.f;// 30 pixels = 1 meter
	static constexpr float physics_scale_inv = 1.0f / physics_scale;

	static constexpr int sub_step_count = 4;//specific box2d parameter (see box2d API)
	static constexpr float time_step = 0.017f; //60 FPS update
};
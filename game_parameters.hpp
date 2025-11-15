#pragma once
#define M_PI 3.14159265358979323846

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

	//these can be changed they are just what was taken from the pacman code - components does call this though, so that will have to be change if you want to remove it.
	static constexpr float small_speed_up = 5.f;
	static constexpr float big_speed_up = 10.f;

	//Player stats
	static constexpr float player_size[2] = { 20.f,30.f };
	static constexpr float player_weight = 10.f;
	static constexpr float player_jump = 40.f;
	static constexpr float player_impulse = 100.f;
	static constexpr float player_max_vel[2] = { 200.f,400.f };
	static constexpr float player_friction = 100.f;
	static constexpr float player_restitution = 0.0f;
};
#pragma once
#define M_PI 3.14159265358979323846
#include <SFML/Graphics.hpp>

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
	static constexpr float player_jump = 200.f;
	static constexpr float player_impulse = 300.f;
	static constexpr float player_max_vel[2] = { 200.f,400.f };
	static constexpr float player_friction = 0.f;
	static constexpr float player_restitution = 0.0f;
	static constexpr float health = 3;


	//Controls
	static constexpr sf::Keyboard::Key move_left = sf::Keyboard::A;
	static constexpr sf::Keyboard::Key move_right = sf::Keyboard::D;
	static constexpr sf::Keyboard::Key move_jump = sf::Keyboard::Space;
	static constexpr sf::Keyboard::Key look_up = sf::Keyboard::W;
	static constexpr sf::Keyboard::Key look_down = sf::Keyboard::S;
	static constexpr sf::Keyboard::Key move_dash = sf::Keyboard::LShift;
	static constexpr sf::Keyboard::Key attack_fire_ball = sf::Keyboard::LControl;
	static constexpr sf::Mouse::Button attack_fire_ball_fire = sf::Mouse::Left;
	static constexpr sf::Keyboard::Key attack_melee = sf::Keyboard::E;

	//Dash properties
	static constexpr float dash_duration = 0.1f;
	static constexpr float dash_speed = 2000.0f;

	//Fireball Properties
	static constexpr float fireball_target_size[2] = { 10.f,10.f };
	static constexpr float fireball_velocity = 500;
	static constexpr float fireball_cooldown = 5;

	//Melee Attack Properties
	static constexpr float time_to_start_attack = 0.2;
	static constexpr float attack_duration = time_to_start_attack + 0.2;
	static constexpr float attack_cooldown = attack_duration + 0.8;
	static constexpr float hitbox_offset = 2.6;
	
	//Enemy Attacks Properties
	static constexpr float enemy_attack_start_range = 15;
	static constexpr float enemy_time_to_start_attack = 0.4;
	static constexpr float enemy_attack_duration = enemy_time_to_start_attack + 0.2;
	static constexpr float enemy_attack_cooldown = enemy_attack_duration + 1;
	static constexpr float knockmack_duration = 0.07;
	static constexpr float knockback_force[2] = { 2500, 200 };

	//Enemy Properties
	static constexpr float enemy_size[2] = { 20.f,30.f };
	static constexpr float enemy_weight = 10.f;
	static constexpr float enemy_friction = 0.f;
	static constexpr float enemy_restitution = 0.0f;
	static constexpr int enemy_health = 3; // Takes 3 hits to put to sleep

	//Enemy Attack Properties
	//static constexpr float enemy_detection_range = 300.f; // How far enemy can detect player
	//static constexpr float enemy_attack_range = 40.f; // How close to player to attack
	//static constexpr float enemy_attack_damage = 10; // Damage per hit
	//static constexpr float enemy_attack_cooldown = 2.0f; // 2 seconds between attacks
	//static constexpr float enemy_attack_startup = 0.3f; // Wind-up before damage
	//static constexpr float enemy_attack_duration = 0.5f; // Full attack animation
	//static constexpr float enemy_move_speed = 80.f; // Movement speed toward player

	//Sleep/ZZZ Properties
	static constexpr int zzz_font_size = 24;
	static constexpr float zzz_offset_x = 0.f; // Horizontal offset from enemy center
	static constexpr float zzz_offset_y = -45.f; // Vertical offset (above enemy)
	static constexpr float sleep_fade_time = 2.0f; // How long enemy stays visible after sleeping

};
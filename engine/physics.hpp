#pragma once
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "level_system.hpp"

class Physics
{
public:
	static void initialise(); //initialise the box2d world
	static void shutdown(); //close the physics engine
	static void update(const float&);//step the simulation

	static b2WorldId get_world_id();

	static b2ContactEvents get_contact_events();// get all the contact events at the current time step
	static b2SensorEvents get_sensor_events();// get all the sensor events at the current time step

	//Convert from b2Vec2 to a Vector2f
	static const sf::Vector2f bv2_to_sv2(const b2Vec2& in);
	//Convert from Vector2f to a b2Vec2
	static const b2Vec2 sv2_to_bv2(const sf::Vector2f& in);
	//Convert from screenspace.y to physics.y (as they are the other way around)
	static const sf::Vector2f invert_height(const sf::Vector2f& in, const int& game_height);

	//General parameters related to the physic engine
	static constexpr float physics_scale = 30.0f; //30 pixels = 1 meter
	static constexpr float physics_scale_inv = 1.0f / physics_scale;
	static constexpr int sub_step_count = 4; //specific box2d parameter (see box2d API)
	static constexpr float time_step = 0.017f; //60FPS update
	static constexpr float gravity = -9.8f; //gravity constant 
private:
	static b2WorldId _world_id;
};
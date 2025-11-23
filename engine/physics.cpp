#include "physics.hpp"

//using ls = LevelSystem;

b2WorldId Physics::_world_id;

void Physics::initialise()
{
	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = b2Vec2({ 0.0f, gravity });
	_world_id = b2CreateWorld(&world_def);
}

void Physics::shutdown()
{
	b2DestroyWorld(_world_id);
}

void Physics::update(const float& time_step)
{
	b2World_Step(_world_id, time_step, sub_step_count);
}

b2WorldId Physics::get_world_id()
{
	return _world_id;
}

b2ContactEvents Physics::get_contact_events()
{
	return b2World_GetContactEvents(_world_id);
}

/*
* Functions to allow Box2D to talk with SFML
* - Box2D and SFML don't use the same units or orientation
* - Box2D uses metres - (0,0) is the bottom left of the screen - y increases upwards
* - SFML uses pixels - (0,0) is the top left of the screen - y increases downwards
*/
//Convert from b2Vec2 to a Vector2f
const sf::Vector2f Physics::bv2_to_sv2(const b2Vec2& in)
{
	return sf::Vector2f(in.x * physics_scale, in.y * physics_scale);
}
//Convert from Vector2f to a b2Vec2
const b2Vec2 Physics::sv2_to_bv2(const sf::Vector2f& in)
{
	return { in.x * physics_scale_inv, in.y * physics_scale_inv };
}
//Convert from screenspace.y to physics.y (as they are the other way around)
const sf::Vector2f Physics::invert_height(const sf::Vector2f& in, const int& game_height)
{
	return sf::Vector2f(in.x, game_height - in.y);
}


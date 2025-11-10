#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

namespace box2d
{
	//Convert from b2Vec2 to a Vector2f
	const sf::Vector2f bv2_to_sv2(const b2Vec2& in);
	//Convert from Vector2f to a b2Vec2
	const b2Vec2 sv2_to_bv2(const sf::Vector2f& in);
	//Convert from screenspace.y to physics.y (as they are the other way around)
	const sf::Vector2f invert_height(const sf::Vector2f& in);

	//Create a Box2D body with a box fixture
	b2BodyId create_physics_box(b2WorldId& world_id, const bool dynamic, const sf::Vector2f& position, const sf::Vector2f& size);
	b2BodyId create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs);

}//box2d
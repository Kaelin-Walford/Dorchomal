#include "b2_utils.hpp"
#include "game_parameters.hpp"

using param = Parameters;

/*
* Functions to allow Box2D to talk with SFML
* - Box2D and SFML don't use the same units or orientation
* - Box2D uses metres - (0,0) is the bottom left of the screen - y increases upwards
* - SFML uses pixels - (0,0) is the top left of the screen - y increases downwards
*/
//Convert from b2Vec2 to a Vector2f
const sf::Vector2f box2d::bv2_to_sv2(const b2Vec2& in)
{
	return sf::Vector2f(in.x * param::physics_scale, in.y * param::physics_scale);
}
//Convert from Vector2f to a b2Vec2
const b2Vec2 box2d::sv2_to_bv2(const sf::Vector2f& in)
{
	return { in.x * param::physics_scale_inv, in.y * param::physics_scale_inv };
}
//Convert from screenspace.y to physics.y (as they are the other way around)
const sf::Vector2f box2d::invert_height(const sf::Vector2f& in)
{
	return sf::Vector2f(in.x, param::game_height - in.y);
}

//Create a Box2D body with a box fixture
b2BodyId box2d::create_physics_box(b2WorldId& world_id, const bool dynamic, const sf::Vector2f& position, const sf::Vector2f& size)
{
	b2BodyDef body_def = b2DefaultBodyDef();
	//Is Dynamic(moving), or static(Stationary) - the ? is a short-hand if else statement where it returns the first value if true and second if false
	// i.e. if dynamic is true then it returns b2_dynamicBody, if false then b2_staticBody
	body_def.type = dynamic ? b2_dynamicBody : b2_staticBody;
	body_def.position = sv2_to_bv2(position);
	//Create the body
	b2BodyId body_id = b2CreateBody(world_id, &body_def);

	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	//density affects the mass of the object
	shape_def.density = dynamic ? 10.f : 0.f;
	//Friction afects the objects ability to slide
	shape_def.material.friction = dynamic ? 0.8f : 1.f;
	//Restitution is the bounciness of the object
	shape_def.material.restitution = 0.0f;

	//This defines the shape of the box2D, its halved cause Box2D uses half width and height
	b2Polygon polygon = b2MakeBox(sv2_to_bv2(size).x * 0.5f, sv2_to_bv2(size).y * 0.5f);
	b2CreatePolygonShape(body_id, &shape_def, &polygon);

	return body_id;
}

//Allows for the SFML shape to be passed in instead of passing the position and size - it then passes the SFML size and position to the main create_physics_box function
b2BodyId box2d::create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs)
{
	return create_physics_box(world_id, dynamic, rs->getPosition(), rs->getSize());
}



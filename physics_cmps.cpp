#include <iostream>
#include "physics_cmps.hpp"
#include "physics.hpp"
#include "game_parameters.hpp"
#include "level_system.hpp"
#include <algorithm>
#include <cmath>

using param = Parameters;
using ph = Physics;
//using ls = LevelSystem;

/*
*	Platform Component
*/

PlatformComponent::PlatformComponent(Entity* p, const std::vector<sf::Vector2i>& tile_group,
	float friction, float restitution)
	: Component(p), _friction(friction), _restitution(restitution)
{
	//This creates the static body
	b2BodyDef body_def = b2DefaultBodyDef();
	body_def.type = b2_staticBody;
	//Create the body
	_body_id = b2CreateBody(ph::get_world_id(), &body_def);
	//_create_chain_shape(tile_group);
}
void PlatformComponent::update(const float& dt) {}
void PlatformComponent::render() {}

//Destructor cleans up the platorm component
PlatformComponent::~PlatformComponent()
{
	b2DestroyChain(_chain_id);
	_chain_id = b2_nullChainId;
	b2DestroyBody(_body_id);
	_body_id = b2_nullBodyId;
}

//This is commented out as it requires the level_system.cpp
/*
void PlatformComponent::_create_chain_shape(const std::vector<sf::Vector2i> &tile_group){
	std::vector<b2Vec2> points;
	for(int i = 0; i < tile_group.size(); i++){
		const sf::Vector2i &tile = tile_group[i];
		std::vector<ls::Tile> neighbors = {
			ls::in_group({tile.x-1,tile.y-1},tile_group) ? ls::get_tile({tile.x-1,tile.y-1}) : ls::EMPTY,
			ls::in_group({tile.x,tile.y-1},tile_group) ? ls::get_tile({tile.x,tile.y-1}) : ls::EMPTY,
			ls::in_group({tile.x+1,tile.y-1},tile_group) ? ls::get_tile({tile.x+1,tile.y-1}) : ls::EMPTY,
			ls::in_group({tile.x+1,tile.y},tile_group) ? ls::get_tile({tile.x+1,tile.y}) : ls::EMPTY,
			ls::in_group({tile.x+1,tile.y+1},tile_group) ? ls::get_tile({tile.x+1,tile.y+1}) : ls::EMPTY,
			ls::in_group({tile.x,tile.y+1},tile_group) ? ls::get_tile({tile.x,tile.y+1}) : ls::EMPTY,
			ls::in_group({tile.x-1,tile.y+1},tile_group) ? ls::get_tile({tile.x-1,tile.y+1}) : ls::EMPTY,
			ls::in_group({tile.x-1,tile.y},tile_group) ? ls::get_tile({tile.x-1,tile.y}) : ls::EMPTY
		};
		sf::Vector2f pos = ls::get_tile_position(tile);

		std::vector<sf::Vector2f> pts;
		if(neighbors[0] == ls::EMPTY || neighbors[1] == ls::EMPTY || neighbors[7] == ls::EMPTY)
			pts.push_back(pos);
		if(neighbors[1] == ls::EMPTY || neighbors[2] == ls::EMPTY || neighbors[3] == ls::EMPTY)
			pts.push_back({pos.x+param::tile_size,pos.y});
		if(neighbors[3] == ls::EMPTY || neighbors[4] == ls::EMPTY || neighbors[5] == ls::EMPTY)
			pts.push_back({pos.x+param::tile_size,pos.y+param::tile_size});
		if(neighbors[5] == ls::EMPTY || neighbors[6] == ls::EMPTY || neighbors[7] == ls::EMPTY)
			pts.push_back({pos.x,pos.y+param::tile_size});

		for(const sf::Vector2f &pt: pts){
			b2Vec2 point = ph::sv2_to_bv2(ph::invert_height(pt,param::game_height));
			bool already_in = false;
			for(const b2Vec2 &p : points){
				if(p.x == point.x && p.y == point.y){
					already_in = true;
					break;
				}
			}
			if(!already_in)
				points.push_back(point);
		}
	}
	b2Vec2 centroid = {0,0};
	for(const b2Vec2 pt: points){
		centroid.x += pt.x;
		centroid.y += pt.y;
	}
	centroid.x /= static_cast<float>(points.size());
	centroid.y /= static_cast<float>(points.size());
	//order the list of points in counter clockwise.
	std::sort(points.begin(),points.end(),[&](b2Vec2 a, b2Vec2 b){
		a = {a.x-centroid.x,a.y-centroid.y};
		b = {b.x-centroid.x,b.y-centroid.y};
		float angle1 = std::atan2(a.x,a.y);
		float angle2 = std::atan2(b.x,b.y);
		if(angle1==angle2)
			return std::sqrt(a.x*a.x+a.y*a.y)>std::sqrt(b.x*b.x+b.y*b.y);
		else
			return angle1>angle2;
	});
	points.push_back(points.front());

	b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
	material.friction = _friction;
	material.restitution = _restitution;
	b2ChainDef chain_def = b2DefaultChainDef();
	chain_def.count = points.size();
	chain_def.points = points.data();
	chain_def.isLoop = true;
	chain_def.materials = &material;
	chain_def.materialCount = 1;
	_chain_id = b2CreateChain(_body_id,&chain_def);
	std::vector<b2ShapeId> shape_ids(points.size());
	int nbr_seg = b2Chain_GetSegments(_chain_id,shape_ids.data(),points.size());
	shape_ids.size();
}*/

/*
*	Physics Component
*/
//The physics component is the core physics component used by the player and enemies
//update makes sure the visual element follows the physics one
void PhysicsComponent::update(const float& dt)
{
	//std::cout << dt << std::endl;
	_parent->set_position(ph::invert_height(ph::bv2_to_sv2(b2Body_GetPosition(_body_id)),
		param::game_height));
	_parent->set_rotation((180 / M_PI) * b2Rot_GetAngle(b2Body_GetRotation(_body_id)));
}

PhysicsComponent::PhysicsComponent(Entity* p, bool dyn)
	: Component(p), _dynamic(dyn)
{
	//
	b2BodyDef body_def = b2DefaultBodyDef();
	//Is Dynamic(moving), or static(Stationary) - the ? is a short-hand if else statement where it returns the first value if true and second if false
	// i.e. if dynamic is true then it returns b2_dynamicBody, if false then b2_staticBody
	body_def.type = _dynamic ? b2_dynamicBody : b2_staticBody;
	body_def.position = ph::sv2_to_bv2(ph::invert_height(_parent->get_position(), param::game_height));

	//Create the body
	_body_id = b2CreateBody(ph::get_world_id(), &body_def);
}

//Restitution is the bounciness of the object
void PhysicsComponent::set_restitution(float r)
{
	_restitution = r;
	b2Shape_SetRestitution(_shape_id, r);
}

//Friction afects the objects ability to slide
void PhysicsComponent::set_friction(float f)
{
	_friction = f;
	b2Shape_SetFriction(_shape_id, f);
}

//density affects the mass of the object
void PhysicsComponent::set_mass(float m)
{
	_mass = m;
	b2Shape_SetDensity(_shape_id, m, true);
}

void PhysicsComponent::teleport(const sf::Vector2f& v)
{
	b2CosSin cos_sin = b2ComputeCosSin(0.0f);
	b2Rot rot;
	rot.c = cos_sin.cosine;
	rot.s = cos_sin.sine;
	b2Body_SetTransform(_body_id, ph::sv2_to_bv2(ph::invert_height(v, param::game_height)), rot);
}

//returns the velocity of the object
const sf::Vector2f PhysicsComponent::get_velocity() const
{
	return ph::bv2_to_sv2(b2Body_GetLinearVelocity(_body_id));
}

//sets the velocity of the object
void PhysicsComponent::set_velocity(const sf::Vector2f& v)
{
	b2Body_SetLinearVelocity(_body_id, ph::sv2_to_bv2(v));
}

//returns the gravity scale of the object
const float PhysicsComponent::get_gravity_scale() const
{
	return b2Body_GetGravityScale(_body_id);
}

//Sets the gravity scale of the object
void PhysicsComponent::set_gravity_scale(float gravity)
{
	b2Body_SetGravityScale(_body_id, gravity);
}

const b2ShapeId& PhysicsComponent::get_shape_id() const { return _shape_id; }

//The physics component destructor
PhysicsComponent::~PhysicsComponent()
{
	b2DestroyShape(_shape_id, true);
	_shape_id = b2_nullShapeId;
	b2DestroyBody(_body_id);
	_body_id = b2_nullBodyId;
}

void PhysicsComponent::render() {}

//Adds a force to the object
void PhysicsComponent::impulse(const sf::Vector2f& i)
{
	b2Vec2 a;
	a.x = i.x;
	//Y is inverted to match SFML
	a.y = i.y * -1.f;
	b2Body_ApplyLinearImpulseToCenter(_body_id, a, true);
	auto vel = b2Body_GetLinearVelocity(_body_id);
}

//progressivlely decrease velocity if no impulse is applied
void PhysicsComponent::dampen(const sf::Vector2f& i)
{
	auto vel = b2Body_GetLinearVelocity(_body_id);
	vel.x *= i.x;
	vel.y *= i.y;
	b2Body_SetLinearVelocity(_body_id, vel);
}

//This returns all the contacts for the body
int PhysicsComponent::get_contacts(std::array<b2ContactData, 10>& contacts) const
{
	int contact_count = b2Body_GetContactData(_body_id, contacts.data(), 10);
	return contact_count;
}

//Function to create a box shape
void PhysicsComponent::create_box_shape(const sf::Vector2f& size, float mass, float friction, float restitution)
{
	_mass = mass;
	_friction = friction;
	_restitution = restitution;
	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = _dynamic ? _mass : 0.f;
	shape_def.material.friction = _friction;
	shape_def.material.restitution = _restitution;
	b2Polygon polygon = b2MakeBox(ph::sv2_to_bv2(size).x * 0.5f, ph::sv2_to_bv2(size).y * 0.5f);
	_shape_id = b2CreatePolygonShape(_body_id, &shape_def, &polygon);
}

//function to create a capsule shape
void PhysicsComponent::create_capsule_shape(const sf::Vector2f& size, float mass, float friction, float restitution)
{
	_mass = mass;
	_friction = friction;
	_restitution = restitution;
	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = _dynamic ? _mass : 0.f;
	shape_def.material.friction = _friction;
	shape_def.material.restitution = _restitution;
	b2Vec2 b2_size = ph::sv2_to_bv2(size);
	b2Capsule capsule;
	capsule.center1 = { 0,b2_size.y * 0.5f - b2_size.x * 0.5f };
	capsule.center2 = { 0,-b2_size.y * 0.5f + b2_size.x * 0.5f };
	capsule.radius = b2_size.x * 0.5f;
	_shape_id = b2CreateCapsuleShape(_body_id, &shape_def, &capsule);
}

/*
*	Player Physics Component
*/

PlayerPhysicsComponent::PlayerPhysicsComponent(Entity* p, const sf::Vector2f& size) : PhysicsComponent(p, true)
{
	_size = ph::sv2_to_bv2(size);
	_max_velocity = sf::Vector2f(param::player_max_vel[0], param::player_max_vel[1]);
	_ground_speed = param::player_impulse;
	_grounded = false;
	_can_dash = true;
	_is_dashing = false;
	_facing_right = true;
	_dash_current_duration = 0.f;

	//Prevents the player from rotating and sleeping
	b2Body_EnableSleep(_body_id, false);
	b2Body_SetFixedRotation(_body_id, true);
	//Bullet items have higher-res collision detection
	// b2Body_SetBullet(_body_id,true);

	//sets the friction, mass and restitution of the player
	//float friction = 0.0f;
	//float mass = 5.0f;
	//float restitution = 0.0f;
	//set_friction(friction);
	//set_mass(mass);
	//set_restitution(restitution);
}

//This checks if the collision normal is pointing upward, which means the player is on the ground
bool PlayerPhysicsComponent::is_grounded() const
{
	std::array<b2ContactData, 10> contacts;
	int count = get_contacts(contacts);
	if (count <= 0)
	{
		//std::cout << "IN THE AIR 1";
		return false;
	}
	const b2Vec2& pos = b2Body_GetPosition(_body_id);
	const float half_y = _size.y * .5f;
	for (int i = 0; i < count; i++)
	{
		if (contacts[i].manifold.normal.y == 1)
		{
			//std::cout << "IS GROUNDED";
			return true;
		}
	}

	//std::cout << "IN THE AIR 2";
	return false;
}


void PlayerPhysicsComponent::update(const float& dt)
{
	//std::cout << _friction << "\n";
	const sf::Vector2f pos = _parent->get_position();
	b2Vec2 b2_pos = ph::sv2_to_bv2(ph::invert_height(pos, param::game_height));

	//Check if the player in in the air or not
	_grounded = is_grounded();

	//std::cout << _restitution << "\n";

	//Teleport to start if we fall off map.
//Curently Commented out until level_system is implemented
/*
if (pos.y > ls::get_height() * param::tile_size) {
	teleport(ls::get_start_position());
}*/

	//check to only allow player movement while they are not dashing
	//dstd::cout << get_gravity_scale();
	if (!_is_dashing)
	{
		//Handles left and right movement
		if (sf::Keyboard::isKeyPressed(param::move_left) ||
			sf::Keyboard::isKeyPressed(param::move_right))
		{
			// Moving Either Left or Right
			if (sf::Keyboard::isKeyPressed(param::move_right))
			{
				//impulse({ (dt * _ground_speed), 0 });
				set_velocity(sf::Vector2f(_ground_speed, get_velocity().y));
			}
			else
			{
				//impulse({ -(dt * _ground_speed), 0 });
				set_velocity(sf::Vector2f(-_ground_speed, get_velocity().y));
			}
		}
		else
		{
			// Dampen X axis movement
			//dampen({ 0.9f, 1.0f });
			//Stop moving the player left or right when there is to imput pressed
			set_velocity(sf::Vector2f(0, get_velocity().y));
		}

		// Clamp velocity.
		sf::Vector2f v = get_velocity();
		//Clamp y for terminal velocity
		//v.x = copysign(std::min(abs(v.x), _max_velocity.x), v.x);
		v.y = copysign(std::min(abs(v.y), _max_velocity.y), v.y);
		set_velocity(v);
		//std::cout << "should not happen when dashing\n";

		//Handles the dash
		if (_can_dash)
		{
			if (sf::Keyboard::isKeyPressed(param::move_dash))
			{
				//std::cout << "enterdash";
				//angle dashes if the user presses multiple directions
				if (sf::Keyboard::isKeyPressed(param::move_left) && sf::Keyboard::isKeyPressed(param::look_up))
				{
					dash(false, true);
				}
				else if (sf::Keyboard::isKeyPressed(param::move_left) && sf::Keyboard::isKeyPressed(param::look_down))
				{
					dash(false, false);
				}
				else if (sf::Keyboard::isKeyPressed(param::move_right) && sf::Keyboard::isKeyPressed(param::look_up))
				{
					dash(true, true);
				}
				else if (sf::Keyboard::isKeyPressed(param::move_right) && sf::Keyboard::isKeyPressed(param::look_down))
				{
					dash(true, false);
				}

				//dashes in one directions
				else if (sf::Keyboard::isKeyPressed(param::look_up))
				{
					set_velocity(sf::Vector2f(get_velocity().x, param::dash_speed));
				}
				else if (sf::Keyboard::isKeyPressed(param::move_right))
				{
					set_velocity(sf::Vector2f(param::dash_speed, get_velocity().y));
				}
				else if (sf::Keyboard::isKeyPressed(param::look_down))
				{
					set_velocity(sf::Vector2f(get_velocity().x, -param::dash_speed));
				}
				else if (sf::Keyboard::isKeyPressed(param::move_left))
				{
					set_velocity(sf::Vector2f(-param::dash_speed, get_velocity().y));
				}

				//dash in the direction of the direction the player is facing if they weren't holding a direction
				else
				{
					//if statment to get velocity to apply in the direction the player is facing
					float dash_speed = _facing_right ? param::dash_speed : -param::dash_speed;
					set_velocity(sf::Vector2f(dash_speed, get_velocity().y));
				}

				_is_dashing = true;
				_can_dash = false;
				set_gravity_scale(0.f);
			}
		}

		// Handle Jump
		if (sf::Keyboard::isKeyPressed(param::move_jump))
		{
			//_grounded = is_grounded();
			if (_grounded)
			{
				set_velocity(sf::Vector2f(get_velocity().x, 0.f));
				teleport(sf::Vector2f(pos.x, pos.y - 2.0f));
				impulse(sf::Vector2f(0, -param::player_jump));
			}
		}

		
	}
	else
	{
		//std::cout << get_velocity().x << "     " << get_velocity().y << "\n";
		//std::cout << dt << "\n";
		//std::cout << _dash_current_duration << "\n";
		_dash_current_duration += dt;
		if (_dash_current_duration >= param::dash_duration)
		{
			_is_dashing = false;
			_dash_current_duration = 0.f;
			set_gravity_scale(1.f);
		}
	}

	//Are we in air?
	if (!_grounded)
	{
		// Check to see if we have landed yet
		_grounded = is_grounded();
		// disable friction while jumping
		set_friction(0.f);
	}
	else
	{
		//std::cout << "Change the friction";
		set_friction(param::player_friction);
		//Allows the player to dash again
		_can_dash = true;
	}

	PhysicsComponent::update(dt);
}

//Function for the player to dash in a direction provided
void PlayerPhysicsComponent::dash(bool rightSide, bool topSide)
{
	float angle = M_PI / 4; //45 degrees
	float hypotenuse = sqrt(pow(param::dash_speed, 2) + pow(param::dash_speed, 2) - (2 * param::dash_speed * param::dash_speed * cos(angle)));
	std::cout << hypotenuse << "    " << cos(angle) << "\n";

	float x = cos(angle) * hypotenuse;
	float y = topSide ? x : -x;
	x = rightSide ? x : -x;

	std::cout << x << "			" << y << "\n";

	set_velocity(sf::Vector2f(x, y));
}

/*
* used for creating the walls for test scenes
* should be deleted not required for final game
*/

//Create a Box2D body with a box fixture
b2BodyId testSceneBox2D::create_physics_box(b2WorldId& world_id, const bool dynamic, const sf::Vector2f& position, const sf::Vector2f& size)
{
	b2BodyDef body_def = b2DefaultBodyDef();
	//Is Dynamic(moving), or static(Stationary) - the ? is a short-hand if else statement where it returns the first value if true and second if false
	// i.e. if dynamic is true then it returns b2_dynamicBody, if false then b2_staticBody
	body_def.type = dynamic ? b2_dynamicBody : b2_staticBody;
	body_def.position = Physics::sv2_to_bv2(position);
	//Create the body
	b2BodyId body_id = b2CreateBody(world_id, &body_def);

	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	//density affects the mass of the object
	shape_def.density = dynamic ? 10.f : 0.f;
	//Friction afects the objects ability to slide
	shape_def.material.friction = dynamic ? 0.8f : 1.f;
	//Restitution is the bounciness of the object
	shape_def.material.restitution = 1.0f;

	//This defines the shape of the box2D, its halved cause Box2D uses half width and height
	b2Polygon polygon = b2MakeBox(Physics::sv2_to_bv2(size).x * 0.5f, Physics::sv2_to_bv2(size).y * 0.5f);
	b2CreatePolygonShape(body_id, &shape_def, &polygon);

	return body_id;
}

//Allows for the SFML shape to be passed in instead of passing the position and size - it then passes the SFML size and position to the main create_physics_box function
b2BodyId testSceneBox2D::create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs)
{
	return create_physics_box(world_id, dynamic, rs->getPosition(), rs->getSize());
}
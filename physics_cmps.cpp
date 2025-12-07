#include <iostream>
#include "physics_cmps.hpp"
#include "physics.hpp"
#include "game_parameters.hpp"
#include "level_system.hpp"
#include "game_system.hpp"
#include "graphics_cmps.hpp"
#include "renderer.hpp"
#include <algorithm>
#include <cmath>
#include "audio_system.hpp"

#include "UI/game_settings.hpp"
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
	_parent->set_position(ph::invert_height(ph::bv2_to_sv2(b2Body_GetPosition(_body_id)),
		param::game_height));
	_parent->set_rotation((180 / M_PI) * b2Rot_GetAngle(b2Body_GetRotation(_body_id)));

	//update the entities shape
	_entities.update(dt);
}

PhysicsComponent::PhysicsComponent(Entity* p, bool dyn) : Component(p), _dynamic(dyn)
{
	b2BodyDef body_def = b2DefaultBodyDef();
	//Is Dynamic(moving), or static(Stationary) - the ? is a short-hand if else statement where it returns the first value if true and second if false
	// i.e. if dynamic is true then it returns b2_dynamicBody, if false then b2_staticBody
	body_def.type = _dynamic ? b2_dynamicBody : b2_kinematicBody;
	body_def.position = ph::sv2_to_bv2(ph::invert_height(_parent->get_position(), param::game_height));
	//Sets userdata to TEMP so each physics component has userdata - should be overriden by the class that inherits from this
	body_def.userData = "TEMP";

	//Create the body
	_body_id = b2CreateBody(ph::get_world_id(), &body_def);

	_can_use_fireball = true;
	_can_attack = true;
	attacking = false;
	_has_attacked = false;
	in_range_of_target = false;
	_shape_destroyed = false;

	//Sounds
	_attack_sound.add_sound("Slash.wav");
	_walk_sound.add_sound("walk.wav");
	_fireball_sound.add_sound("Slash.wav");
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

//Instantly moves the body to a new location
void PhysicsComponent::teleport(const sf::Vector2f& v)
{
	b2CosSin cos_sin = b2ComputeCosSin(0.0f);
	b2Rot rot;
	rot.c = cos_sin.cosine;
	rot.s = cos_sin.sine;
	b2Body_SetTransform(_body_id, ph::sv2_to_bv2(ph::invert_height(v, param::game_height)), rot);
}

//returns the position of the physics object
const sf::Vector2f PhysicsComponent::get_position() const
{
	return  _parent->get_position();
}

//Returns the box2d position
const b2Vec2 PhysicsComponent::get_b2_position() const
{
	const sf::Vector2f vec = get_position();
	return ph::sv2_to_bv2(ph::invert_height(vec, param::game_height));
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
	if (!_shape_destroyed && b2Shape_IsValid(_attack_hitbox_shape_id))
	{
		b2DestroyShape(_attack_hitbox_shape_id, true);
		_attack_hitbox_shape_id = b2_nullShapeId;
	}
	if (b2Shape_IsValid(_shape_id))
	{
		b2DestroyShape(_shape_id, true);
		_shape_id = b2_nullShapeId;
	}
	b2DestroyBody(_body_id);
	_body_id = b2_nullBodyId;
}

//Renders the _entities
void PhysicsComponent::render()
{
	_entities.render();
}

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
void PhysicsComponent::create_box_shape(const sf::Vector2f& size, float mass, float friction, float restitution, int filter, char* userdata)
{
	_mass = mass;
	_friction = friction;
	_restitution = restitution;

	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = _dynamic ? _mass : 0.f;
	shape_def.material.friction = _friction;
	shape_def.material.restitution = _restitution;

	//defines the filter group so the body only interacts or doesnt interact with other members of the group
	shape_def.filter.groupIndex = _filter;

	//allows contact and sensor events
	shape_def.enableContactEvents = true;
	shape_def.enableSensorEvents = true;
	shape_def.userData = (void*)userdata;
	b2Polygon polygon = b2MakeBox(ph::sv2_to_bv2(size).x * 0.5f, ph::sv2_to_bv2(size).y * 0.5f);
	_shape_id = b2CreatePolygonShape(_body_id, &shape_def, &polygon);
}

//function to create a capsule shape
void PhysicsComponent::create_capsule_shape(const sf::Vector2f& size, float mass, float friction, float restitution, int filter, char* userdata)
{
	_mass = mass;
	_friction = friction;
	_restitution = restitution;
	_filter = filter;

	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.density = _dynamic ? _mass : 0.f;
	shape_def.material.friction = _friction;
	shape_def.material.restitution = _restitution;

	//defines the filter group so the body only interacts or doesnt interact with other members of the group
	shape_def.filter.groupIndex = _filter;

	//allows contact and sensor events
	shape_def.enableContactEvents = true;
	shape_def.enableSensorEvents = true;
	shape_def.userData = (void*)userdata;
	b2Vec2 b2_size = ph::sv2_to_bv2(size);
	b2Capsule capsule;
	capsule.center1 = { 0,b2_size.y * 0.5f - b2_size.x * 0.5f };
	capsule.center2 = { 0,-b2_size.y * 0.5f + b2_size.x * 0.5f };
	capsule.radius = b2_size.x * 0.5f;
	_shape_id = b2CreateCapsuleShape(_body_id, &shape_def, &capsule);
}

//Attack hitbox
void PhysicsComponent::create_attack_hitbox(const sf::Vector2f& size)
{
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.filter.groupIndex = _filter;

	shape_def.userData = "Melee";
	shape_def.enableSensorEvents = true;
	//shape_def.posit

	//sets it to be a sensor so it only detects other objects entering it with no collisions
	shape_def.isSensor = true;

	//b2Vec2 b2_offset = { ph::sv2_to_bv2(offset).x, ph::sv2_to_bv2(offset).y};

	b2Polygon polygon = b2MakeBox(ph::sv2_to_bv2(size).x * 2, ph::sv2_to_bv2(size).y * 0.5);
	_attack_hitbox_shape_id = b2CreatePolygonShape(_body_id, &shape_def, &polygon);
}

//Function to create an entity
const std::shared_ptr<Entity>& PhysicsComponent::make_entity()
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	_entities.list.push_back(entity);
	return _entities.list.back();
}

//Function to return the user data for the body
const void* PhysicsComponent::get_user_data() const
{
	return b2Body_GetUserData(_body_id);
}

//Function to return the user data for the shape
const void* PhysicsComponent::get_shape_user_data() const
{
	return b2Shape_GetUserData(_shape_id);
}

//This checks if the collision normal is pointing upward, which means the player is on the ground
bool PhysicsComponent::is_grounded() const
{
	std::array<b2ContactData, 10> contacts;
	int count = get_contacts(contacts);
	if (count <= 0)
	{
		return false;
	}
	const b2Vec2& pos = b2Body_GetPosition(_body_id);
	const float half_y = _size.y * .5f;
	for (int i = 0; i < count; i++)
	{
		if (contacts[i].manifold.normal.y == 1)
		{
			return true;
		}
	}

	return false;
}

//Effect and GetComponent properties
//reduce the components health dy damage
void PhysicsComponent::reduce_health(int damage)
{
	_health -= damage;
}

//Attacks
//Attack Timer
void PhysicsComponent::attack_timer(const float& dt)
{
	if (!_can_attack)
	{
		//Duration till entity can attack again
		_attack_wait_timer += dt;
		if (_attack_wait_timer >= _attack_cooldown)
		{
			_can_attack = true;
			_attack_wait_timer = 0;
		}

		//When the enemy has finished attacking
		else if (_attack_wait_timer >= _attack_duration)
		{
			attacking = false;
			_has_attacked = false;
		}

		//When the entity can start to attack
		else if (_attack_wait_timer >= _time_to_start_attack)
		{
			if (!_has_attacked)
			{
				attacking = true;
				_has_attacked = true;
			}
		}
	}
}

//create fireball
void PhysicsComponent::fireball(sf::Vector2f velocity, float rotation, sf::Vector2f position)
{
	//create the fireball
	std::shared_ptr<Entity> fireball;

	fireball = make_entity();
	fireball->set_position(sf::Vector2f(position.x, position.y - 1));
	fireball->set_visible(true);

	std::shared_ptr<ShapeComponent> shape = fireball->add_component<ShapeComponent>();
	shape->set_shape<sf::RectangleShape>(sf::Vector2f(param::player_size[0], param::player_size[1]));
	shape->get_shape().setFillColor(sf::Color::Red);
	shape->get_shape().setOrigin(sf::Vector2f(param::player_size[0] / 2.f, param::player_size[1] / 2.f));

	std::shared_ptr<FireballComponent> fireball_component = fireball->add_component<FireballComponent>(sf::Vector2f(position.x, position.y - 1), velocity, rotation);
	fireball_component->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), b2Shape_GetFilter(_shape_id).groupIndex);

	_can_use_fireball = false;
}

//Knockback the entity
void PhysicsComponent::knockback_entity(float dt)
{
	//runs while being knockedback
	_knockback_duration += dt;
	if (_knockback_duration >= param::knockmack_duration)
	{
		knockback = false;
		_knockback_duration = 0;
		set_velocity(sf::Vector2f(0, 0));
		set_gravity_scale(1);
	}
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
	knockback = false;
	facing_right = true;
	_dash_current_duration = 0.f;
	_fireball_wait_timer = 0.0f;
	_health = param::health;
	_just_dashed = false;
	_knockback_duration = 0;

	_attack_cooldown = param::attack_cooldown;
	_attack_duration = param::attack_duration;
	_time_to_start_attack = param::time_to_start_attack;

	//Sounds
	_dash_sound.add_sound("dash.wav");

	//define the fireball target shape
	_target = make_entity();
	_target->set_position(sf::Vector2f(0, 0));
	_target->set_visible(false);

	std::shared_ptr<ShapeComponent> shape = _target->add_component<ShapeComponent>();
	shape->set_shape<sf::RectangleShape>(sf::Vector2f(param::fireball_target_size[0], param::fireball_target_size[1]));
	shape->get_shape().setFillColor(sf::Color::Magenta);

	//Prevents the player from rotating and sleeping
	b2Body_EnableSleep(_body_id, false);
	b2Body_SetFixedRotation(_body_id, true);
	b2Body_SetUserData(_body_id, "Player");

	create_attack_hitbox(sf::Vector2f(param::player_size[0], param::player_size[1]));

	//_attack_hitbox_left_shape_id b2shapedisa
	//Bullet items have higher-res collision detection
	// b2Body_SetBullet(_body_id,true);
}

void PlayerPhysicsComponent::update(const float& dt)
{
	auto& settings = GameSettings::get_instance();
	const sf::Vector2f pos = _parent->get_position();
	b2Vec2 b2_pos = ph::sv2_to_bv2(ph::invert_height(pos, param::game_height));

	//Check if the player in in the air or not
	_grounded = is_grounded();


	//Teleport to start if we fall off map.
	//Curently Commented out until level_system is implemented
	/*
	if (pos.y > ls::get_height() * param::tile_size) {
	teleport(ls::get_start_position());
	}*/

	//Fireball timer
	if (!_can_use_fireball)
	{
		_fireball_wait_timer += dt;
		if (_fireball_wait_timer >= param::fireball_cooldown)
		{
			_can_use_fireball = true;
			_fireball_wait_timer = 0;
		}
	}


	//Attack Timer
	attack_timer(dt);

	//check to only allow player movement while they are not dashing
	if (!_is_dashing && !knockback)
	{
		//Are we in air?
		if (!_grounded)
		{
			// disable friction while jumping
			set_friction(0.f);
		}
		else
		{
			set_friction(param::player_friction);
			//Allows the player to dash again
			_can_dash = true;
		}

		//Handles left and right movement
		if (sf::Keyboard::isKeyPressed(settings.key_move_left) || sf::Keyboard::isKeyPressed(settings.key_move_right))
		{
			if (get_gravity_scale() == 0)
			{
				set_gravity_scale(1);
			}
			// Moving Either Left or Right
			if (sf::Keyboard::isKeyPressed(settings.key_move_right))
			{
				set_velocity(sf::Vector2f(_ground_speed, get_velocity().y));
				if (!facing_right)
				{
					facing_right = true;
				}

			}
			else
			{
				set_velocity(sf::Vector2f(-_ground_speed, get_velocity().y));
				if (facing_right)
				{
					facing_right = false;
				}
			}
			_just_dashed = false;
			//If on the ground and not playing a sound then play walk sound
			if (_grounded)
			{
				if (_walk_sound.is_sound_playing() == sf::SoundSource::Stopped)
				{
					_walk_sound.play_sound();
				}
			}
		}
		else if (!_just_dashed)
		{
			//Stop moving the player left or right when there is to imput pressed
			set_velocity(sf::Vector2f(0, get_velocity().y));
		}
		//If the player has dashed or been knocked back and haven't done any movement they won't be brought to an abrupt stop
		else
		{
			if (get_velocity().x > 0)
			{
				set_velocity(sf::Vector2f(get_velocity().x - 100, get_velocity().y - 100));
				if (get_velocity().x <= 0)
				{
					set_velocity(sf::Vector2f(0, 0));
					_just_dashed = false;
					set_gravity_scale(1);
				}

			}
			else
			{
				set_velocity(sf::Vector2f(get_velocity().x + 100, get_velocity().y + 100));
				if (get_velocity().x >= 0)
				{
					set_velocity(sf::Vector2f(0, 0));
					_just_dashed = false;
					set_gravity_scale(1);
				}
			}
		}

		// Clamp velocity.
		sf::Vector2f v = get_velocity();
		//Clamp y for terminal velocity
		v.y = copysign(std::min(abs(v.y), _max_velocity.y), v.y);
		set_velocity(v);

		//Handles the dash
		if (_can_dash)
		{
			if (sf::Keyboard::isKeyPressed(settings.key_dash))
			{
				//Play dash sound
				_fireball_sound.play_sound();

				//angle dashes if the user presses multiple directions
				if (sf::Keyboard::isKeyPressed(settings.key_move_left) && sf::Keyboard::isKeyPressed(settings.key_look_up))
				{
					dash(false, true);
				}
				else if (sf::Keyboard::isKeyPressed(settings.key_move_left) && sf::Keyboard::isKeyPressed(settings.key_look_down))
				{
					dash(false, false);
				}
				else if (sf::Keyboard::isKeyPressed(settings.key_move_right) && sf::Keyboard::isKeyPressed(settings.key_look_up))
				{
					dash(true, true);
				}
				else if (sf::Keyboard::isKeyPressed(settings.key_move_right) && sf::Keyboard::isKeyPressed(settings.key_look_down))
				{
					dash(true, false);
				}

				//dashes in one directions
				else if (sf::Keyboard::isKeyPressed(settings.key_look_up))
				{
					set_velocity(sf::Vector2f(get_velocity().x, param::dash_speed));
				}
				else if (sf::Keyboard::isKeyPressed(settings.key_move_right))
				{
					set_velocity(sf::Vector2f(param::dash_speed, get_velocity().y));
				}
				else if (sf::Keyboard::isKeyPressed(settings.key_look_down))
				{
					set_velocity(sf::Vector2f(get_velocity().x, -param::dash_speed));
				}
				else if (sf::Keyboard::isKeyPressed(settings.key_move_left))
				{
					set_velocity(sf::Vector2f(-param::dash_speed, get_velocity().y));
				}

				//dash in the direction of the direction the player is facing if they weren't holding a direction
				else
				{
					//if statment to get velocity to apply in the direction the player is facing
					float dash_speed = facing_right ? param::dash_speed : -param::dash_speed;
					set_velocity(sf::Vector2f(dash_speed, get_velocity().y));
				}

				_is_dashing = true;
				_can_dash = false;
				set_gravity_scale(0.f);
			}
		}

		// Handle Jump
		if (sf::Keyboard::isKeyPressed(settings.key_jump))
		{
			if (_grounded)
			{
				set_velocity(sf::Vector2f(get_velocity().x, param::player_jump));
				teleport(sf::Vector2f(pos.x, pos.y - 2.0f));
			}
		}

		// Handle Fireball
		if (sf::Keyboard::isKeyPressed(settings.key_fireball))
		{
			//Displays the target on the screen
			_target->set_visible(true);
			_target->set_position(sf::Vector2f(GameSystem::get_mouse_position()));
			if (sf::Mouse::isButtonPressed(settings.mouse_fireball) && _can_use_fireball)
			{
				//play fireball sound
				_fireball_sound.play_sound();

				//get the velocity for the fireball
				auto [velocity, rotation] = fireball_direction(_target->get_position(), pos);

				fireball(velocity, rotation, pos);
			}
		}
		else
		{
			_target->set_visible(false);
		}

		//Handle Melee Attack
		if (sf::Keyboard::isKeyPressed(settings.key_melee))
		{
			if (_can_attack)
			{
				_attack_wait_timer = 0.f;
				_can_attack = false;
			}
		}
	}
	else if (_is_dashing)
	{
		//Runs while dashing
		_dash_current_duration += dt;
		if (_dash_current_duration >= param::dash_duration)
		{
			_is_dashing = false;
			_dash_current_duration = 0.f;
			set_velocity(sf::Vector2f(0, 30));
			set_gravity_scale(1);
		}
	}
	else
	{
		knockback_entity(dt);
	}


	//Delete fireballs
	for (auto& entity : get_entities())
	{
		auto components = entity->get_components<FireballComponent>();
		for (auto& component : components)
		{
			if (component->is_for_deletion())
			{
				entity->set_for_delete();
			}
		}
	}



	PhysicsComponent::update(dt);
}

//Function for the player to dash in a direction provided
void PlayerPhysicsComponent::dash(bool rightSide, bool topSide)
{
	float angle = M_PI / 4; //45 degrees
	//Use the formula c^2 = a^2 + b^2 - 2ab Cos0 - 0 is theta
	float hypotenuse = sqrt(pow(param::dash_speed, 2) + pow(param::dash_speed, 2) - (2 * param::dash_speed * param::dash_speed * cos(angle)));

	//Use SohCahToa to calculate the x and y velocity - topside and rightsie is used to determine wether x or y is negative
	float x = cos(angle) * hypotenuse;
	float y = topSide ? x : -x;
	x = rightSide ? x : -x;

	set_velocity(sf::Vector2f(x, y));
}

//Function for the player to cast a fireball
std::tuple<sf::Vector2f, float> PlayerPhysicsComponent::fireball_direction(sf::Vector2f target_position, sf::Vector2f player_position)
{
	//Player position
	float a = player_position.x;
	float b = player_position.y;

	//target position
	float x = target_position.x;
	float y = target_position.y;

	float by = b - y;
	float xa = x - a;

	//gets the angle to send the fireball towards - by using SohCahToa
	float angle = atan((b - y) / (x - a));

	if (angle < 0)
	{
		angle = -angle;
	}

	//gets the x and y velocity
	float velocityY = param::fireball_velocity * sin(angle);
	float velocityX = param::fireball_velocity * cos(angle);

	if (by < 0)
	{
		velocityY = -velocityY;
	}
	if (xa < 0)
	{
		velocityX = -velocityX;
	}

	//rotaion for the fireball
	float fireball_rotation = (M_PI / 2) - angle;
	if (by < 0 && xa < 0)
	{
		fireball_rotation += M_PI;
	}
	else if (by < 0)
	{
		fireball_rotation += M_PI;
		fireball_rotation = -fireball_rotation;

	}
	else if (xa < 0)
	{
		fireball_rotation = -fireball_rotation;
	}

	return { sf::Vector2f(velocityX, velocityY), fireball_rotation };
}

/*
*	Enemy Attack Physics Component
*/

EnemyAttackComponent::EnemyAttackComponent(Entity* p, Entity* player, const sf::Vector2f& size, int type) : PhysicsComponent(p, true)
{
	_size = ph::sv2_to_bv2(size);

	player_in_range = false;
	in_range_of_player = false;
	_player = player;
	attacking = false;
	_attack_cooldown = param::enemy_attack_cooldown;
	_attack_duration = param::enemy_attack_duration;
	_time_to_start_attack = param::enemy_time_to_start_attack;
	_fireball_wait_timer = 0;
	facing_right = false;
	knockback = false;
	_knockback_duration = 0;
	_can_move = true;
	defeated = false;

	//1 - enemy without attacks - 2 melee attacks enemy - 3 fireball attack enemy
	_enemy_type = type;

	if (_enemy_type == 1)
	{
		_health = 1;
		_default_colour = sf::Color::Cyan;
		_chasing_colour = sf::Color(0, 150, 255);
	}
	else if (_enemy_type == 2)
	{
		_health = 3;

		create_attack_hitbox(sf::Vector2f(param::player_size[0], param::player_size[1]));
		_default_colour = sf::Color(255, 105, 180);
		_chasing_colour = sf::Color(255, 182, 193);
	}
	else
	{
		_health = 2;
		_default_colour = sf::Color::Green;
		_chasing_colour = sf::Color(0, 255, 150);
	}

	//set enemy colour
	auto shape_components = _parent->get_components<ShapeComponent>();
	shape_components[0]->get_shape().setFillColor(_default_colour);

	// Initialize sleep system
	_is_asleep = false;
	_sleep_timer = 0.f;
	_font_loaded = false;

	// Try to load font for ZZZ text
	if (_zzz_font.loadFromFile("resources/fonts/pixelated.otf"))
	{
		_font_loaded = true;
		_zzz_text.setFont(_zzz_font);
		_zzz_text.setString("ZZZ");
		_zzz_text.setCharacterSize(param::zzz_font_size);
		_zzz_text.setFillColor(sf::Color::White);
		_zzz_text.setOutlineColor(sf::Color::Black);
		_zzz_text.setOutlineThickness(1.f);

		// Center the text
		sf::FloatRect textBounds = _zzz_text.getLocalBounds();
		_zzz_text.setOrigin(textBounds.left + textBounds.width / 2.f,
			textBounds.top + textBounds.height / 2.f);
	}

	//Prevents the enemy from rotating and sleeping
	b2Body_EnableSleep(_body_id, false);
	b2Body_SetFixedRotation(_body_id, true);
	b2Body_SetUserData(_body_id, "Enemy");
}

void EnemyAttackComponent::update(const float& dt)
{
	const sf::Vector2f pos = _parent->get_position();
	b2Vec2 b2_pos = ph::sv2_to_bv2(ph::invert_height(pos, param::game_height));

	if (!knockback)
	{
		if (!_is_asleep)
		{
			if (_enemy_type == 2)
			{
				attack_timer(dt);

				//if the enemy is in range of the player to start attacking them
				if (x_distance(param::enemy_attack_start_range) && in_range_of_player)
				{
					//Stop moving
					set_velocity(sf::Vector2f(0.f, get_velocity().y));

					bool facing_player = false;
					if ((_player->get_position().x < get_position().x) && !facing_right)
					{
						facing_player = true;
					}
					if ((_player->get_position().x > get_position().x) && facing_right)
					{
						facing_player = true;
					}
					if (facing_player)
					{
						if (_can_attack)
						{
							_attack_wait_timer = 0.f;
							_can_attack = false;
						}
					}
				}
			}

			if (_enemy_type == 3)
			{
				//Fireball timer
				if (!_can_use_fireball)
				{
					_fireball_wait_timer += dt;
					if (_fireball_wait_timer >= param::fireball_cooldown)
					{
						_fireball_wait_timer = 0;
						_can_use_fireball = true;
					}
				}
				else
				{
					int velocity = param::fireball_velocity;
					if (_player->get_position().x < get_position().x)
					{
						velocity = -velocity;
						fireball(sf::Vector2f(velocity, 0), (3 * M_PI / 2), pos);
					}
					else
					{
						fireball(sf::Vector2f(velocity, 0), (M_PI / 2), pos);
					}
				}

				//Delete fireballs
				for each(std::shared_ptr<Entity> entity in get_entities())
				{
					auto components = entity->get_components<FireballComponent>();
					for each(std::shared_ptr<FireballComponent> component in components)
					{
						if (component->is_for_deletion())
						{
							entity->set_for_delete();
						}
					}
				}

				//if the enemy is within stopping range
				if (get_distance_to_player() <= param::fireball_enemy_stopping_range)
				{
					_can_move = false;
				}
				else
				{
					_can_move = true;
				}
			}
		}
	}
	// If enemy is asleep
	if (_is_asleep)
	{
		_sleep_timer += dt;

		// Update ZZZ text position
		if (_font_loaded)
		{
			sf::Vector2f enemy_pos = _parent->get_position();
			_zzz_text.setPosition(enemy_pos.x + param::zzz_offset_x,
				enemy_pos.y + param::zzz_offset_y);

			// Animate ZZZ (bob up and down slightly)
			float bob = sin(_sleep_timer * 2.0f) * 3.0f;
			_zzz_text.move(0.f, bob * dt);
		}

		// Gradually fade out after sleep_fade_time
		if (_sleep_timer > param::sleep_fade_time)
		{
			auto shape_components = _parent->get_components<ShapeComponent>();
			if (!shape_components.empty())
			{
				sf::Color current = shape_components[0]->get_shape().getFillColor();
				int alpha = std::max(0, static_cast<int>(current.a - 100 * dt));
				current.a = alpha;
				shape_components[0]->get_shape().setFillColor(current);

				// Also fade ZZZ text
				if (_font_loaded)
				{
					sf::Color text_color = _zzz_text.getFillColor();
					text_color.a = alpha;
					_zzz_text.setFillColor(text_color);
				}

				// Mark for deletion when fully faded
				if (alpha <= 0)
				{
					defeated = true;
				}
			}
		}

		if (knockback)
		{
			//runs while being knockedback
			_knockback_duration += dt;
			if (_knockback_duration >= param::knockmack_duration)
			{
				knockback = false;
				_knockback_duration = 0;
				set_velocity(sf::Vector2f(0, 0));
				set_gravity_scale(1);
			}
		}

		PhysicsComponent::update(dt);
		return; // Don't do any other behavior while asleep
	}
	else if (!knockback)
	{

		// Handle being put to sleep (health reaches 0)
		if (_health <= 0)
		{
			put_to_sleep();
			return;
		}

		const sf::Vector2f pos = _parent->get_position();

		// Check if player exists
		if (_player)
		{
			float distance = get_distance_to_player();

			// Visual feedback - change color based on state
			auto shape_components = _parent->get_components<ShapeComponent>();
			if (!shape_components.empty())
			{
				if (!_can_attack)
				{
					shape_components[0]->get_shape().setFillColor(sf::Color(255, 20, 147)); // Light red when attacking
				}
				else if (distance <= param::enemy_detection_range)
				{
					shape_components[0]->get_shape().setFillColor(_chasing_colour); // Orange when chasing
				}
				else
				{
					shape_components[0]->get_shape().setFillColor(_default_colour); // Normal red when idle
				}
			}

			// State machine
			if ((distance <= param::enemy_detection_range) && (_can_move == true))
			{
				// CHASE STATE
				if (_can_attack)
				{
					move_toward_player(dt);
				}
			}
			else
			{
				// IDLE STATE
				// Stop moving
				set_velocity(sf::Vector2f(0.f, get_velocity().y));
			}
		}

		PhysicsComponent::update(dt);
	}
	else
	{
		knockback_entity(dt);
	}
}

// Calculate distance to player
float EnemyAttackComponent::get_distance_to_player() const
{
	if (!_player)
		return 999999.f; // Return large number if no player

	sf::Vector2f enemy_pos = _parent->get_position();
	sf::Vector2f player_pos = _player->get_position();

	float dx = player_pos.x - enemy_pos.x;
	float dy = player_pos.y - enemy_pos.y;

	return std::sqrt(dx * dx + dy * dy);
}

// Move enemy toward player
void EnemyAttackComponent::move_toward_player(const float& dt)
{
	if (!_player)
		return;

	sf::Vector2f enemy_pos = _parent->get_position();
	sf::Vector2f player_pos = _player->get_position();

	// Calculate direction
	float dx = player_pos.x - enemy_pos.x;
	float dy = player_pos.y - enemy_pos.y;
	float distance = std::sqrt(dx * dx + dy * dy);

	if (distance > 0.f)
	{
		// Normalize and apply speed
		dx /= distance;
		dy /= distance;

		// Only move horizontally, let gravity handle vertical
		sf::Vector2f current_vel = get_velocity();
		set_velocity(sf::Vector2f(dx * param::enemy_move_speed, current_vel.y));
		is_moving = true;
	}
}

// Put enemy to sleep
void EnemyAttackComponent::put_to_sleep()
{
	if (!_is_asleep)
	{
		_is_asleep = true;
		_sleep_timer = 0.f;

		// Stop all movement
		set_velocity(sf::Vector2f(0.f, 0.f));
		set_gravity_scale(0.f); // Float in place

		// Change color to indicate sleep
		auto shape_components = _parent->get_components<ShapeComponent>();
		if (!shape_components.empty())
		{
			shape_components[0]->get_shape().setFillColor(sf::Color(100, 100, 255, 200)); // Light blue, semi-transparent
		}
		//destroy main shape
		b2Body_Disable(_body_id);
	}
}



//Returns true if  the distance between this enemy and the player only based on the x axis is less than or equal to the provided distance
bool EnemyAttackComponent::x_distance(int distance)
{
	int x_distance = get_position().x - _player->get_components<PlayerPhysicsComponent>()[0]->get_position().x - param::player_size[0];
	if (x_distance < 0)
	{
		x_distance = !x_distance;
	}
	if (x_distance <= distance)
	{
		return true;
	}
	else
	{
		return false;
	}
}


// Render the ZZZ text if asleep
void EnemyAttackComponent::render()
{
	PhysicsComponent::render();

	if (_is_asleep && _font_loaded)
	{
		Renderer::queue(&_zzz_text);
	}
}

/*
*	Fireball Physics Component
*/

FireballComponent::FireballComponent(Entity* p, sf::Vector2f position, sf::Vector2f velocity, float angle) : Component(p)
{
	b2BodyDef body_def = b2DefaultBodyDef();
	//Is Dynamic(moving), or static(Stationary) - the ? is a short-hand if else statement where it returns the first value if true and second if false
	// i.e. if dynamic is true then it returns b2_dynamicBody, if false then b2_staticBody
	body_def.type = b2_dynamicBody;
	body_def.position = ph::sv2_to_bv2(ph::invert_height(_parent->get_position(), param::game_height));
	body_def.userData = "Fireball";
	body_def.rotation = b2MakeRot(angle);

	//Create the body
	_body_id = b2CreateBody(ph::get_world_id(), &body_def);
	b2Body_SetGravityScale(_body_id, 0.0f);

	//set the velocity for the fireball
	b2Body_SetLinearVelocity(_body_id, ph::sv2_to_bv2(velocity));
}

void FireballComponent::update(const float& dt)
{
	//checks if the fireball has hit something, if so then destroy itself
	std::array<b2ContactData, 10> contacts;
	int count = get_contacts(contacts);
	if (count > 0)
	{
		_for_deletion = true;
	}

	_parent->set_position(ph::invert_height(ph::bv2_to_sv2(b2Body_GetPosition(_body_id)),
		param::game_height));
	_parent->set_rotation((180 / M_PI) * b2Rot_GetAngle(b2Body_GetRotation(_body_id)));
}

void FireballComponent::render()
{

}

//gets the contacts of the fireball
int FireballComponent::get_contacts(std::array<b2ContactData, 10>& contacts) const
{
	int contact_count = b2Body_GetContactData(_body_id, contacts.data(), 10);
	return contact_count;
}

//Function to create a box shape
void FireballComponent::create_box_shape(const sf::Vector2f& size, int filter_group)
{
	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.filter.groupIndex = filter_group;
	shape_def.enableContactEvents;
	shape_def.userData = "Fireball";
	b2Polygon polygon = b2MakeBox(ph::sv2_to_bv2(size).x * 0.5f, ph::sv2_to_bv2(size).y * 0.5f);
	_shape_id = b2CreatePolygonShape(_body_id, &shape_def, &polygon);
}

//function to create a capsule shape
void FireballComponent::create_capsule_shape(const sf::Vector2f& size, int filter_group)
{
	//Create the fixture shape
	b2ShapeDef shape_def = b2DefaultShapeDef();
	shape_def.filter.groupIndex = filter_group;
	shape_def.enableContactEvents;
	shape_def.userData = "Fireball";
	b2Vec2 b2_size = ph::sv2_to_bv2(size);
	b2Capsule capsule;
	capsule.center1 = { 0,b2_size.y * 0.5f - b2_size.x * 0.5f };
	capsule.center2 = { 0,-b2_size.y * 0.5f + b2_size.x * 0.5f };
	capsule.radius = b2_size.x * 0.5f;
	_shape_id = b2CreateCapsuleShape(_body_id, &shape_def, &capsule);
}

FireballComponent::~FireballComponent()
{
	b2DestroyShape(_shape_id, true);
	_shape_id = b2_nullShapeId;
	b2DestroyBody(_body_id);
	_body_id = b2_nullBodyId;
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
	shape_def.material.restitution = 0.0f;

	//shape_def.enableSensorEvents = true;

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
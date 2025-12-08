#pragma once
#include "ecm.hpp"
#include <array>
#include <box2d/box2d.h>
#include "audio_system.hpp"

//forward declaration
class FireballComponent;

//The platform component is all the static geometry in the world
class PlatformComponent : public Component
{
public:
	PlatformComponent(Entity* p, const std::vector<sf::Vector2i>& tile_group, float friction = 40.f, float restitution = 0.2f);
	void update(const float& dt) override;
	void render() override;
	const b2ChainId& get_chain_id() const;
	~PlatformComponent() override;

protected:
	b2BodyId _body_id;
	b2ChainId _chain_id;
	float _friction;
	float _restitution;
	void _create_chain_shape(const std::vector<sf::Vector2i> &tile_group);
};

//The physics component is the main class that the player and enemy classes will inherit from
class PhysicsComponent : public Component
{
public:
	PhysicsComponent(Entity* p, bool dyn);
	const b2ShapeId& get_shape_id() const;
	int get_contacts(std::array<b2ContactData, 10>& contacts) const;
	const sf::Vector2f get_velocity() const;
	const float get_gravity_scale() const;
	const sf::Vector2f get_position() const;
	const b2Vec2 get_b2_position() const;
	void set_restitution(float r);
	void set_friction(float r);
	void set_mass(float m);
	void update(const float& dt) override;
	void render() override;
	void impulse(const sf::Vector2f& i);
	void dampen(const sf::Vector2f& s);
	void set_velocity(const sf::Vector2f& v);
	void set_gravity_scale(float gravity);
	void teleport(const sf::Vector2f& v);
	void create_box_shape(const sf::Vector2f& size, float mass, float friction, float restitution, int filter, char* userdata);
	void create_capsule_shape(const sf::Vector2f& size, float mass, float friction, float restitution, int filter, char* userdata);
	void create_attack_hitbox(const sf::Vector2f& size);
	bool is_grounded() const;

	//entity functions
	const std::shared_ptr<Entity>& make_entity();
	std::vector<std::shared_ptr<Entity>>& get_entities() { return _entities.list; }
	void delete_fireballs();

	//Collision Information
	const void* get_user_data() const;
	const void* get_shape_user_data() const;

	//Effect and Get Component properties
	void reduce_health(int damage);
	int get_health() { return _health; }

	//attacks
	void attack_timer(const float& dt);
	void fireball(sf::Vector2f velocity, float rotation, sf::Vector2f position);
	void knockback_entity(float dt);

	//Allows the scene to know when the entity is attacking
	bool attacking;
	bool in_range_of_target;

	bool facing_right;

	bool knockback;


	~PhysicsComponent() override;
protected:
	b2BodyId _body_id;
	b2ShapeId _shape_id;
	b2ShapeId _attack_hitbox_shape_id;
	bool _shape_destroyed;
	const bool _dynamic;
	float _friction;
	float _restitution;
	float _mass;
	int _filter;
	bool _can_use_fireball;
	float _fireball_wait_timer;
	int _health;
	bool _grounded;
	b2Vec2 _size;

	//Attack variables
	bool _can_attack;
	float _attack_wait_timer;
	bool _has_attacked;
	float _attack_cooldown;
	float _attack_duration;
	float _time_to_start_attack;
	float _knockback_duration;

	//entities
	EntityManager _entities;
	std::vector<std::shared_ptr<FireballComponent>> _fireball_components;

	//Stores sounds
	AudioSystem _attack_sound;
	AudioSystem _hit_sound;
	AudioSystem _walk_sound;
	AudioSystem _fireball_sound;
};

//The class that handles player controls
class PlayerPhysicsComponent : public PhysicsComponent
{
public:
	void update(const float& dt) override;
	void dash(bool rightSide, bool topSide);
	std::tuple<sf::Vector2f, float> fireball_direction(sf::Vector2f target_position, sf::Vector2f player_position);

	explicit PlayerPhysicsComponent(Entity* p, const sf::Vector2f& size);

	PlayerPhysicsComponent() = delete;
protected:
	
	sf::Vector2f _max_velocity;
	
	float _ground_speed;
	bool _can_dash;
	float _dash_current_duration;
	bool _is_dashing;
	bool _just_dashed;

	std::shared_ptr<Entity> _target;

	//Sounds
	AudioSystem _dash_sound;
};

//The class used to create an enemy that can attack - will be merge with enemy movement
class EnemyAttackComponent : public PhysicsComponent
{
public:
	void update(const float& dt) override;
	explicit EnemyAttackComponent(Entity* p, Entity* player, const sf::Vector2f& size, int type);
	void render() override;
	explicit EnemyAttackComponent(Entity* p, const sf::Vector2f& size);

	EnemyAttackComponent() = delete;

	//true if the player can hit the enemy
	bool player_in_range;
	//true if the enemy can hit the player
	bool in_range_of_player;
	//bool to check if the enemy is moving
	bool is_moving;
	//if the enemy is defeated
	bool defeated;

	//Gets the distance between this enemy and the player on the x axis
	bool x_distance(int distance);
	// Set player reference for AI
	//void set_player_entity(std::shared_ptr<Entity> player);
	//bool is_attacking() const { return _is_attacking; }
	bool is_asleep() const { return _is_asleep; }

	// Sleep system
	void put_to_sleep();

protected:
	//1 - enemy without attacks - 2 melee attacks enemy - 3 fireball attack enemy
	int _enemy_type;
	//Entity* _player;
	b2Vec2 _size;

	// Player tracking
	Entity* _player;

	//if enemy can move
	bool _can_move;

	//previous health
	int _previous_health;

	// Sleep state
	bool _is_asleep;
	sf::Text _zzz_text;
	sf::Font _zzz_font;
	float _sleep_timer;
	bool _font_loaded;
	sf::Color _default_colour;
	sf::Color _chasing_colour;
	
	//Sounds
	AudioSystem _damage_sound;

	// Helper functions
	float get_distance_to_player() const;
	void move_toward_player(const float& dt);
	//void perform_attack(const float& dt);
};

//The class used to create a fireball
class FireballComponent : public Component
{
public:
	FireballComponent(Entity* p, sf::Vector2f position, sf::Vector2f velocity, float angle);
	int get_contacts(std::array<b2ContactData, 10>& contacts) const;
	void update(const float& dt) override;
	void render() override;
	void create_box_shape(const sf::Vector2f& size, int filter_group);
	void create_capsule_shape(const sf::Vector2f& size, int filter_group);

	~FireballComponent() override;

protected:
	b2BodyId _body_id;
	b2ShapeId _shape_id;
};

/*
* used for creating the walls for test scenes
* should be deleted not required for final game
*/

namespace testSceneBox2D
{
	//Create a Box2D body with a box fixture
	b2BodyId create_physics_box(b2WorldId& world_id, const bool dynamic, const sf::Vector2f& position, const sf::Vector2f& size);
	b2BodyId create_physics_box(b2WorldId& world_id, const bool dynamic, const std::shared_ptr<sf::RectangleShape>& rs);
}
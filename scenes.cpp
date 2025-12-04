#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "scenes.hpp"
#include "game_parameters.hpp"
#include "renderer.hpp"
#include "b2_utils.hpp"
#include "graphics_cmps.hpp"
#include "physics_cmps.hpp"
#include "physics.hpp"

//remove
#include "ecm.hpp"

using param = Parameters;
namespace b2 = box2d;

std::shared_ptr<Scene> Scenes::physics;
std::shared_ptr<Scene> Scenes::kaelinsPlayground;

//Load the physics scene, which is a scene that has cubes fall from the sky
void PhysicsScene::load()
{
	//creates the box2D world
	b2WorldDef world_def = b2DefaultWorldDef();
	//sets the gravity for the world
	world_def.gravity = b2Vec2({ 0.0f, param::g });
	//identifies the physics simulation
	world_id = b2CreateWorld(&world_def);


	// Create Boxes
	for (int i = 1; i < 11; ++i)
	{
		// Create SFML shapes for each box
		std::shared_ptr<sf::RectangleShape> s = std::make_shared<sf::RectangleShape>();
		s->setPosition(sf::Vector2f(i * (param::game_width / 12.f), param::game_height * .7f));
		s->setSize(sf::Vector2f(50.0f, 50.0f));
		s->setOrigin(sf::Vector2f(25.0f, 25.0f));
		s->setFillColor(sf::Color::White);
		sprites.push_back(s);

		// Create a dynamic physics body for the box
		b2BodyId b = b2::create_physics_box(world_id, true, s);
		// Give the box a spin
		b2Body_ApplyAngularImpulse(b, 5.0f, true);
		bodies.push_back(b);
	}

	sf::Vector2f walls[] = {
		// Top
		sf::Vector2f(param::game_width * .5f, 5.f), sf::Vector2f(param::game_width, 10.f),
		// Bottom
		sf::Vector2f(param::game_width * .5f, param::game_height - 5.f), sf::Vector2f(param::game_width, 10.f),
		// left
		sf::Vector2f(5.f, param::game_height * .5f), sf::Vector2f(10.f, param::game_height),
		// right
		sf::Vector2f(param::game_width - 5.f, param::game_height * .5f), sf::Vector2f(10.f, param::game_height)
	};

	// Build Walls
	for (int i = 0; i < 7; i += 2)
	{
		// Create SFML shapes for each wall
		std::shared_ptr<sf::RectangleShape> s = std::make_shared<sf::RectangleShape>();
		s->setPosition(walls[i]);
		s->setSize(walls[i + 1]);
		s->setOrigin(walls[i + 1] / 2.f);
		s->setFillColor(sf::Color::White);
		sprites.push_back(s);

		// Create a static physics body for the wall
		b2BodyId b = b2::create_physics_box(world_id, false, s);
		bodies.push_back(b);
	}
}

//The update function for the physics scene
void PhysicsScene::update(const float& dt)
{
	// Step Physics world by time_step
	b2World_Step(world_id, param::time_step, param::sub_step_count);

	for (int i = 0; i < bodies.size(); ++i)
	{
		// Sync Sprites to physics position
		sprites[i]->setPosition(b2::invert_height(b2::bv2_to_sv2(b2Body_GetPosition(bodies[i]))));
		// Sync Sprites to physics Rotation
		sprites[i]->setRotation((180 / M_PI) * asin(b2Body_GetRotation(bodies[i]).s));
	}
}

//Adds the physics scene to the renderer queue
void PhysicsScene::render()
{
	for (std::shared_ptr<sf::RectangleShape> sprite : sprites)
	{
		Renderer::queue(sprite.get());
	}

}

//clears the scene
void PhysicsScene::unload()
{
	for (std::shared_ptr<sf::RectangleShape>& shape : sprites)
	{
		shape.reset();
	}
	sprites.clear();

	for (b2BodyId body : bodies)
	{
		b2DestroyBody(body);
	}
	bodies.clear();
	b2DestroyWorld(world_id);
}

/*
* Kaelins Playground Scene
* This is just a testing scene for all my components
*/

//Loads the Playground scene
void KaelinsPlayground::load()
{
	scene_restart = false;


	sf::Vector2f walls[] = {
		// Top
		sf::Vector2f(param::game_width * .5f, 5.f), sf::Vector2f(param::game_width, 10.f),
		// Bottom
		sf::Vector2f(param::game_width * .5f, param::game_height - 5.f), sf::Vector2f(param::game_width, 10.f),
		// left
		sf::Vector2f(5.f, param::game_height * .5f), sf::Vector2f(10.f, param::game_height),
		// right
		sf::Vector2f(param::game_width - 5.f, param::game_height * .5f), sf::Vector2f(10.f, param::game_height)
	};


	// Build Walls
	for (int i = 0; i < 7; i += 2)
	{
		// Create SFML shapes for each wall
		std::shared_ptr<sf::RectangleShape> s = std::make_shared<sf::RectangleShape>();
		s->setPosition(walls[i]);
		s->setSize(walls[i + 1]);
		s->setOrigin(walls[i + 1] / 2.f);
		s->setFillColor(sf::Color::White);
		sprites.push_back(s);

		// Create a static physics body for the wall
		b2BodyId b = testSceneBox2D::create_physics_box(Physics::get_world_id(), false, s);
		bodies.push_back(b);
	}

	//Create the player
	_player = make_entity();
	_player->set_position(sf::Vector2f(100.0f, 100.0f));

	std::shared_ptr<ShapeComponent> shape = _player->add_component<ShapeComponent>();
	shape->set_shape<sf::RectangleShape>(sf::Vector2f(param::player_size[0], param::player_size[1]));
	shape->get_shape().setFillColor(sf::Color::Yellow);
	shape->get_shape().setOrigin(sf::Vector2f(param::player_size[0] / 2.f, param::player_size[1] / 2.f));

	std::shared_ptr<PlayerPhysicsComponent> cmp = _player->add_component<PlayerPhysicsComponent>(sf::Vector2f(param::player_size[0], param::player_size[1]));
	cmp->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution, -1, "Player");

	//Create an enemy
	std::shared_ptr<Entity> _enemy = make_entity();
	_enemy->set_position(sf::Vector2f(1800, 900));

	std::shared_ptr<ShapeComponent> shapew = _enemy->add_component<ShapeComponent>();
	shapew->set_shape<sf::RectangleShape>(sf::Vector2f(param::player_size[0], param::player_size[1]));
	shapew->get_shape().setFillColor(sf::Color::Red);
	shapew->get_shape().setOrigin(sf::Vector2f(param::player_size[0] / 2.f, param::player_size[1] / 2.f));

	std::shared_ptr<EnemyAttackComponent> ecmp = _enemy->add_component<EnemyAttackComponent>(_player.get(), sf::Vector2f(param::player_size[0], param::player_size[1]), 2);
	ecmp->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution, -2, "1");

	//test
	//Create an enemy
	std::shared_ptr<Entity> _test = make_entity();
	_test->set_position(sf::Vector2f(1000, 900));

	std::shared_ptr<ShapeComponent> shapet = _test->add_component<ShapeComponent>();
	shapet->set_shape<sf::RectangleShape>(sf::Vector2f(param::player_size[0], param::player_size[1]));
	shapet->get_shape().setFillColor(sf::Color::Red);
	shapet->get_shape().setOrigin(sf::Vector2f(param::player_size[0] / 2.f, param::player_size[1] / 2.f));

	std::shared_ptr<EnemyAttackComponent> tcmp = _test->add_component<EnemyAttackComponent>(_player.get(), sf::Vector2f(param::player_size[0], param::player_size[1]), 3);
	tcmp->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution, -2, "2");

	player_user_data = cmp->get_user_data();
	enemy_user_data = ecmp->get_user_data();

	_enemies.push_back(_enemy);
	_enemies.push_back(_test);
}

void KaelinsPlayground::update(const float& dt)
{
	//restarts the scene if its set to restart
	if (scene_restart)
	{
		unload();
		load();
	}
	else
	{
		//set up the contact events and get the number of events
		b2ContactEvents contact_event = Physics::get_contact_events();
		int number_of_contact_events = contact_event.beginCount;

		//set up the sensor events and get the number of events
		b2SensorEvents sensor_event = Physics::get_sensor_events();
		int number_of_sensor_events_start = sensor_event.beginCount;
		int number_of_sensor_events_end = sensor_event.endCount;

		//get the player component
		auto player = _player->get_components<PlayerPhysicsComponent>();

		//loop through each contact event
		for (int i = 0; i < number_of_contact_events; i++)
		{
			//get current event
			b2ContactBeginTouchEvent* begin_contact_event = contact_event.beginEvents + i;
			char* body_1 = (char*)b2Body_GetUserData(b2Shape_GetBody(begin_contact_event->shapeIdA));
			char* body_2 = (char*)b2Body_GetUserData(b2Shape_GetBody(begin_contact_event->shapeIdB));
			char* shape_1 = (char*)b2Shape_GetUserData(begin_contact_event->shapeIdA);
			char* shape_2 = (char*)b2Shape_GetUserData(begin_contact_event->shapeIdB);

			if (body_1 != nullptr && body_2 != nullptr)
			{
				//If the player walks into an enemy
				if ((!strcmp(body_1, "Player") && !strcmp(body_2, "Enemy")) || (!strcmp(body_2, "Player") && !strcmp(body_1, "Enemy")))
				{
					//reduce health and apply knockback
					player[0]->reduce_health(1);
					for (int i = 0; i < _enemies.size(); i++)
					{
						if ((!strcmp(shape_1, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()) || (!strcmp(shape_2, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()))))
						{
							player_knockback(i);
						}
					}
				}

				//If an enemy gets hit by a fireball 
				else if ((!strcmp(body_1, "Fireball") && !strcmp(body_2, "Enemy")) || (!strcmp(body_2, "Fireball") && !strcmp(body_1, "Enemy")))
				{
					find_which_enemy_to_defeat(shape_1, shape_2);
				}
			}
		}
		//Loop through each start sensor events
		for (int i = 0; i < number_of_sensor_events_start; i++)
		{
			b2SensorBeginTouchEvent* begin_sensor_event = sensor_event.beginEvents + i;
			if (b2Shape_IsValid(begin_sensor_event->visitorShapeId) && b2Shape_IsValid(begin_sensor_event->sensorShapeId))
			{
				char* sensor_body = (char*)b2Body_GetUserData(b2Shape_GetBody(begin_sensor_event->sensorShapeId));
				char* visitor_body = (char*)b2Body_GetUserData(b2Shape_GetBody(begin_sensor_event->visitorShapeId));
				char* sensor_shape = (char*)b2Shape_GetUserData(begin_sensor_event->sensorShapeId);
				char* visitor_shape = (char*)b2Shape_GetUserData(begin_sensor_event->visitorShapeId);

				if (sensor_body != nullptr && visitor_body != nullptr)
				{
					//If the player is in melee range of an enemy
					if (!strcmp(sensor_shape, "Melee") && !strcmp(sensor_body, "Player") && !strcmp(visitor_body, "Enemy"))
					{
						find_which_enemy_is_in_range(visitor_shape, true);
						std::cout << "playerstart\n";
					}

					//If the enemey is in melee range of the player
					if (!strcmp(sensor_shape, "Melee") && !strcmp(sensor_body, "Enemy") && !strcmp(visitor_body, "Player"))
					{
						//set to 1 as the main shape will always be in the first spot
						b2ShapeId sensor_shapes[1];
						b2Body_GetShapes(b2Shape_GetBody(begin_sensor_event->sensorShapeId), sensor_shapes, 1);
						find_which_enemy_has_the_player_in_range(sensor_shapes, true);
						std::cout << "enemystart\n";
					}
				}
			}
		}
		//loop through each end sensor events
		for (int i = 0; i < number_of_sensor_events_end; i++)
		{
			b2SensorEndTouchEvent* end_sensor_event = sensor_event.endEvents + i;
			if (b2Shape_IsValid(end_sensor_event->visitorShapeId) && b2Shape_IsValid(end_sensor_event->sensorShapeId))
			{
				char* end_sensor_body = (char*)b2Body_GetUserData(b2Shape_GetBody(end_sensor_event->sensorShapeId));
				char* end_visitor_body = (char*)b2Body_GetUserData(b2Shape_GetBody(end_sensor_event->visitorShapeId));
				char* end_sensor_shape = (char*)b2Shape_GetUserData(end_sensor_event->sensorShapeId);
				char* end_visitor_shape = (char*)b2Shape_GetUserData(end_sensor_event->visitorShapeId);
				if (end_sensor_body != nullptr && end_visitor_body != nullptr)
				{
					//If the player leaves melee range of an enemy
					if (!strcmp(end_sensor_shape, "Melee") && !strcmp(end_sensor_body, "Player") && !strcmp(end_visitor_body, "Enemy"))
					{
						std::cout << "playerend\n";
						find_which_enemy_is_in_range(end_visitor_shape, false);
					}

					//If the enemy leaves melee range of the player
					if (!strcmp(end_sensor_shape, "Melee") && !strcmp(end_sensor_body, "Enemy") && !strcmp(end_visitor_body, "Player"))
					{
						std::cout << "enemyend\n";
						b2ShapeId sensor_shapes[1];
						b2Body_GetShapes(b2Shape_GetBody(end_sensor_event->sensorShapeId), sensor_shapes, 1);
						find_which_enemy_has_the_player_in_range(sensor_shapes, false);
					}
				}
			}
		}

		//if the player attacks an enemy
		if (player[0]->attacking)
		{
			for (int i = 0; i < _enemies.size(); i++)
			{
				//checks that the player is in range with any enemy and deals damage if so
				//std::cout << _enemies[i]->get_components<EnemyAttackComponent>()[0]->player_in_range;
				bool facing_enemy = false;
				if ((_enemies[i]->get_position().x < _player->get_position().x) && !_player->get_components<PlayerPhysicsComponent>()[0]->_facing_right)
				{
					facing_enemy = true;
				}
				else if ((_enemies[i]->get_position().x > _player->get_position().x) && _player->get_components<PlayerPhysicsComponent>()[0]->_facing_right)
				{
					facing_enemy = true;
				}

				if (facing_enemy)
				{
					if (_enemies[i]->get_components<EnemyAttackComponent>()[0]->player_in_range)
					{
						damage_enemy(i, 1);
					}
				}
			}
			player[0]->attacking = false;
		}

		//Enemy Attacks the player
		for (int i = 0; i < _enemies.size(); i++)
		{
			if (_enemies[i]->get_components<EnemyAttackComponent>()[0]->in_range_of_player && _enemies[i]->get_components<EnemyAttackComponent>()[0]->attacking)
			{
				player[0]->reduce_health(2);
				player_knockback(i);
			}
		}

		//If the player dies
		/*if (player[0]->get_health() <= 0)
		{
			scene_restart = true;
		}*/

		Scene::update(dt);

		for (int i = 0; i < _enemies.size(); i++)
		{
			_enemies[i]->get_components<EnemyAttackComponent>()[0]->set_velocity(sf::Vector2f(0, _enemies[i]->get_components<EnemyAttackComponent>()[0]->get_velocity().y));
		}
	}

}

//Function to see which enemy the player defeated
void KaelinsPlayground::find_which_enemy_to_defeat(char* shape_1, char* shape_2)
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		if (!strcmp(shape_1, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()) || !strcmp(shape_2, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()))
		{
			damage_enemy(i, 2);
		}
	}
}

//function to find which enemies are in range
void KaelinsPlayground::find_which_enemy_is_in_range(char* visitor_shape, bool in_range)
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		if (!strcmp(visitor_shape, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()) /* || !strcmp(shape_2, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data())*/)
		{
			_enemies[i]->get_components<EnemyAttackComponent>()[0]->player_in_range = in_range;

		}
	}
}

//Function to find which enemy has the player in range
void KaelinsPlayground::find_which_enemy_has_the_player_in_range(b2ShapeId sensor_shapes[], bool in_range)
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		for (int x = 0; x < sizeof(sensor_shapes[1]); x++)
		{
			if (!strcmp((char*)b2Shape_GetUserData(sensor_shapes[0]), (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()))
			{
				_enemies[i]->get_components<EnemyAttackComponent>()[0]->in_range_of_player = in_range;
			}
		}
	}
}

//Function to damage the enemy
void KaelinsPlayground::damage_enemy(int which_enemy, int damage)
{
	auto enemy = _enemies[which_enemy]->get_components<EnemyAttackComponent>()[0];
	std::cout << enemy->get_health();
	enemy->reduce_health(damage);
	if (enemy->get_health() <= 0)
	{
		_enemies[which_enemy]->set_for_delete();
		_enemies[which_enemy].reset();
		_enemies.erase(_enemies.begin() + which_enemy);
	}
}

//Function to knockback the player
void KaelinsPlayground::player_knockback(int enemy)
{
	_player->get_components<PlayerPhysicsComponent>()[0]->knockback = true;
	_player->get_components<PlayerPhysicsComponent>()[0]->set_gravity_scale(0);
	if (_player->get_position().x < _enemies[enemy]->get_position().x)
	{
		_player->get_components<PlayerPhysicsComponent>()[0]->set_velocity(sf::Vector2f(-param::knockback_force[0], param::knockback_force[1]));
	}
	else
	{
		_player->get_components<PlayerPhysicsComponent>()[0]->set_velocity(sf::Vector2f(param::knockback_force[0], param::knockback_force[1]));
	}
}

void KaelinsPlayground::render()
{
	for (std::shared_ptr<sf::RectangleShape> sprite : sprites)
	{
		Renderer::queue(sprite.get());
	}

	Scene::render();
	_entities.render();
}

void KaelinsPlayground::unload()
{
	Scene::unload();
	_enemies.clear();
	_player.reset();
}
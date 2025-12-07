#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "scenes.hpp"
#include "UI/menu_scene.hpp"
#include "UI/settings_scene.hpp"
#include "UI/credits_scene.hpp" 
#include "UI/game_settings.hpp"
#include "game_parameters.hpp"
#include "renderer.hpp"
#include "b2_utils.hpp"
#include "graphics_cmps.hpp"
#include "physics_cmps.hpp"
#include "physics.hpp"
#include "audio_system.hpp"

//remove
#include "ecm.hpp"

using ls = LevelSystem;
using param = Parameters;
using gs = GameSystem;
namespace b2 = box2d;

std::shared_ptr<MenuScene> Scenes::menuScene;
std::shared_ptr<SettingsScene> Scenes::settingsScene;
std::shared_ptr<CreditsScene> Scenes::creditsScene;
std::shared_ptr<LevelScenes> Scenes::levels;

bool loadLevel2 = false;
bool loadLevel3 = false;

//level loader
void LevelScenes::load() {
	if (loadLevel2 == false && loadLevel3 == false) {
		unload();
		_load_level(param::level_1);
		_current_level_name = "Level 1";
	}
	else if (loadLevel2 == true && loadLevel3 == false) {
		unload();
		_load_level(param::level_2);
		_current_level_name = "Level 2";
	}
	else if (loadLevel2 == true && loadLevel3 == true) {
		unload();
		_load_level(param::level_3);
		_current_level_name = "Level 3";
	}
}

void LevelScenes::_load_level(const std::string& file_path) {
	ls::load_level(file_path, param::tile_size);

	_player = make_entity();
	_player->set_position(ls::get_start_position());

	std::shared_ptr<sf::Texture> _playerTexture = std::make_shared<sf::Texture>();
	if (!_playerTexture->loadFromFile("../../../../resources/textures/practice_sprite_witchgirl.png", sf::IntRect({ 0, 0 }, { 16, 16 }))) {
		std::cerr << "LOAD PLAYER SPRITE ERROR" << std::endl;
	}

	std::shared_ptr<SpriteComponent> playerSprite = _player->add_component<SpriteComponent>();
	playerSprite->set_texure(_playerTexture);
	playerSprite->get_sprite().setOrigin(sf::Vector2f(8, 8));
	playerSprite->get_sprite().setScale(sf::Vector2f(param::player_size[0] / 16, param::player_size[1] / 16));

	// Adds PlayerPhysicsComponent so player can collide with terrain
	std::shared_ptr<PlayerPhysicsComponent> playerPhysics = _player->add_component<PlayerPhysicsComponent>(sf::Vector2f(param::player_size[0], param::player_size[1]));
	playerPhysics->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution, -1, "Player");

	std::vector<std::vector<sf::Vector2i>> wall_groups = ls::get_groups(ls::WALL);
	for (const std::vector<sf::Vector2i>& walls : wall_groups) {
		_walls.push_back(make_entity());
		_walls.back()->add_component<PlatformComponent>(walls);
	}

	//Create an enemy
	std::shared_ptr<Entity> _enemy = make_entity();
	_enemy->set_position(sf::Vector2f(1800, 700));

	std::shared_ptr<ShapeComponent> shapew = _enemy->add_component<ShapeComponent>();
	shapew->set_shape<sf::RectangleShape>(sf::Vector2f(param::enemy_size[0], param::enemy_size[1]));
	shapew->get_shape().setFillColor(sf::Color::Red);
	shapew->get_shape().setOrigin(sf::Vector2f(param::enemy_size[0] / 2.f, param::enemy_size[1] / 2.f));

	std::shared_ptr<EnemyAttackComponent> ecmp = _enemy->add_component<EnemyAttackComponent>(_player.get(), sf::Vector2f(param::player_size[0], param::player_size[1]), 2);
	ecmp->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution, -2, "1");

	//test
	//Create an enemy
	std::shared_ptr<Entity> _test = make_entity();
	_test->set_position(sf::Vector2f(1000, 600));

	std::shared_ptr<ShapeComponent> shapet = _test->add_component<ShapeComponent>();
	shapet->set_shape<sf::RectangleShape>(sf::Vector2f(param::enemy_size[0], param::enemy_size[1]));
	shapet->get_shape().setFillColor(sf::Color::Red);
	shapet->get_shape().setOrigin(sf::Vector2f(param::enemy_size[0] / 2.f, param::enemy_size[1] / 2.f));

	std::shared_ptr<EnemyAttackComponent> tcmp = _test->add_component<EnemyAttackComponent>(_player.get(), sf::Vector2f(param::player_size[0], param::player_size[1]), 3);
	tcmp->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution, -2, "2");

	player_user_data = playerPhysics->get_user_data();
	enemy_user_data = ecmp->get_user_data();

	_enemies.push_back(_enemy);
	_enemies.push_back(_test);
}

void LevelScenes::update(const float& dt) {
	Scene::update(dt);
	_entities.update(dt);
	if (ls::get_tile_at(_player->get_position()) == ls::END && loadLevel2 == false && loadLevel3 == false) {
		unload();
		_load_level(param::level_2);
		loadLevel2 = true;
	}
	else if (ls::get_tile_at(_player->get_position()) == ls::END && loadLevel2 == true && loadLevel3 == false) {
		unload();
		_load_level(param::level_3);
		loadLevel3 = true;
		std::cerr << loadLevel2 << std::endl;
		std::cerr << loadLevel3 << std::endl;
	}
	else if (ls::get_tile_at(_player->get_position()) == ls::END && loadLevel2 == true && loadLevel3 == true) {
		unload();
		Scenes::creditsScene;
	}

	// Handle escape to pause
	static bool escPressed = false;

	if (sf::Keyboard::isKeyPressed(GameSettings::get_instance().key_pause))
	{
		if (!escPressed)
		{
			toggle_pause();
			escPressed = true;
		}
	}
	else
	{
		escPressed = false;
	}

	// Only update game if not paused
	if (!_is_paused)
	{
		//restarts the scene if its set to restart
		if (scene_restart)
		{
			unload();
			load();
			std::cerr << "SCENE RESTARTED" << std::endl;
			scene_restart = false;
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
						find_which_enemy_to_damage(shape_1, shape_2);
					}

					//If the player gets hit by a fireball
					else if ((!strcmp(body_1, "Fireball") && !strcmp(body_2, "Player")) || (!strcmp(body_2, "Fireball") && !strcmp(body_1, "Player")))
					{
						//reduce health
						player[0]->reduce_health(1);
						std::cerr << "PLAYER HURT" << std::endl;
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
						}

						//If the enemey is in melee range of the player
						if (!strcmp(sensor_shape, "Melee") && !strcmp(sensor_body, "Enemy") && !strcmp(visitor_body, "Player"))
						{
							//set to 1 as the main shape will always be in the first spot
							b2ShapeId sensor_shapes[1];
							b2Body_GetShapes(b2Shape_GetBody(begin_sensor_event->sensorShapeId), sensor_shapes, 1);
							find_which_enemy_has_the_player_in_range(sensor_shapes, true);
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
							find_which_enemy_is_in_range(end_visitor_shape, false);
						}

						//If the enemy leaves melee range of the player
						if (!strcmp(end_sensor_shape, "Melee") && !strcmp(end_sensor_body, "Enemy") && !strcmp(end_visitor_body, "Player"))
						{
							//set to 1 as the main shape will always be in the first spot
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
					bool facing_enemy = false;
					if ((_enemies[i]->get_position().x < _player->get_position().x) && !_player->get_components<PlayerPhysicsComponent>()[0]->facing_right)
					{
						facing_enemy = true;
					}
					else if ((_enemies[i]->get_position().x > _player->get_position().x) && _player->get_components<PlayerPhysicsComponent>()[0]->facing_right)
					{
						facing_enemy = true;
					}

					if (facing_enemy)
					{
						if (_enemies[i]->get_components<EnemyAttackComponent>()[0]->player_in_range)
						{
							enemy_knockback(i);
							_enemies[i]->get_components<EnemyAttackComponent>()[0]->reduce_health(1);
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
				defeat_enemy(i);
			}

			//If the player dies
			if (player[0]->get_health() <= 0)
			{
				std::cerr << "PLAYER HEALTH 0" << std::endl;
				scene_restart = true;
			}

			Scene::update(dt);
		}
	}
}

//Function to see which enemy was hit 
void LevelScenes::find_which_enemy_to_damage(char* shape_1, char* shape_2)
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		if (!strcmp(shape_1, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()) || !strcmp(shape_2, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()))
		{
			// Reduce enemy health instead of instant delete
			_enemies[i]->get_components<EnemyAttackComponent>()[0]->reduce_health(1);
		}
	}
}

//function to find which enemies are in range
void LevelScenes::find_which_enemy_is_in_range(char* visitor_shape, bool in_range)
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		if (!strcmp(visitor_shape, (char*)_enemies[i]->get_components<EnemyAttackComponent>()[0]->get_shape_user_data()))
		{
			_enemies[i]->get_components<EnemyAttackComponent>()[0]->player_in_range = in_range;
		}
	}
}

//Function to find which enemy has the player in range
void LevelScenes::find_which_enemy_has_the_player_in_range(b2ShapeId sensor_shapes[], bool in_range)
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
void LevelScenes::defeat_enemy(int which_enemy)
{
	auto enemy = _enemies[which_enemy]->get_components<EnemyAttackComponent>()[0];

	if (enemy->defeated == true)
	{
		_enemies[which_enemy]->set_for_delete();
		_enemies[which_enemy].reset();
		_enemies.erase(_enemies.begin() + which_enemy);
	}
}

//Function to knockback the player
void LevelScenes::player_knockback(int enemy)
{
	auto player = _player->get_components<PlayerPhysicsComponent>()[0];
	player->knockback = true;
	player->set_gravity_scale(0);
	if (_player->get_position().x < _enemies[enemy]->get_position().x)
	{
		player->set_velocity(sf::Vector2f(-param::knockback_force[0], param::knockback_force[1]));
	}
	else
	{
		player->set_velocity(sf::Vector2f(param::knockback_force[0], param::knockback_force[1]));
	}
}

//Function to knockback the enemy provided
void LevelScenes::enemy_knockback(int index)
{
	auto enemy = _enemies[index]->get_components<EnemyAttackComponent>()[0];
	enemy->knockback = true;
	enemy->set_gravity_scale(0);
	if (_enemies[index]->get_position().x < _player->get_position().x)
	{
		enemy->set_velocity(sf::Vector2f(-param::knockback_force[0], param::knockback_force[1]));
	}
	else
	{
		enemy->set_velocity(sf::Vector2f(param::knockback_force[0], param::knockback_force[1]));
	}
}

void LevelScenes::render() {
	ls::render(Renderer::get_window());
	Scene::render();
	_entities.render();

	for (std::shared_ptr<sf::RectangleShape> sprite : sprites)
	{
		Renderer::queue(sprite.get());
	}

	if (_is_paused && Scenes::menuScene)
	{
		Scenes::menuScene->render();
	}
}

void LevelScenes::toggle_pause()
{
	_is_paused = !_is_paused;

	if (Scenes::menuScene)
	{
		if (_is_paused)
		{
			Scenes::menuScene->show_pause_menu(_current_level_name);
			GameSystem::set_active_scene(Scenes::menuScene);
		}
		else
		{
			Scenes::menuScene->hide_menus();
			GameSystem::set_active_scene(Scenes::levels);
		}
	}
}

void LevelScenes::unload() {
	Scene::unload();
	_enemies.clear();
	_player.reset();
	_walls.clear();
}
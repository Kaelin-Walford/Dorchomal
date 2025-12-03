#pragma once
#include "game_system.hpp"
#include "box2d/box2d.h"

class PhysicsScene : public Scene
{
private:
	b2WorldId world_id;
	std::vector<b2BodyId> bodies;
	std::vector<std::shared_ptr<sf::RectangleShape>> sprites;
public:
	PhysicsScene() = default;
	void update(const float& dt) override;
	void render() override;
	void load()override;
	void unload() override;
};

class KaelinsPlayground : public Scene
{
public:
	KaelinsPlayground() = default;
	void update(const float& dt) override;
	void render() override;
	void load()override;
	void unload() override;

	//Collision functions
	void find_which_enemy_to_defeat(char* shape_1, char* shape_2);
	void find_which_enemy_is_in_range(char* visitor_shape, bool in_range);
	void find_which_enemy_has_the_player_in_range(b2ShapeId sensor_shape[], bool inrange);

	//Attack Functions
	void damage_enemy(int which_enemy, int damage);
	void player_knockback(int enemy);
private:
	b2WorldId world_id;
	std::vector<b2BodyId> bodies;
	std::vector<std::shared_ptr<sf::RectangleShape>> sprites;
	std::shared_ptr<Entity> _player;
	std::vector<std::shared_ptr<Entity>> _enemies;
	int timer = 0;
	bool scene_restart;

	//User data for the bodies in the scene
	const void* player_user_data;
	const void* enemy_user_data;
	const void* fireball_user_data;
};

struct Scenes
{
	static std::shared_ptr<Scene> physics;
	static std::shared_ptr<Scene> kaelinsPlayground;
};
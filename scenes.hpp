#pragma once
#include "engine/game_system.hpp"
#include <box2d/box2d.h>

class LevelScenes : public Scene {
public:
	LevelScenes() = default;
	void update(const float& dt) override;
	void render() override;
	void load()override;
	void unload() override;

	void toggle_pause();
	bool is_paused() const { return _is_paused; }
	void set_paused(bool paused) { _is_paused = paused; }
	void reset_to_level_1();  

	void set_current_level(const std::string& level_name) { _current_level_name = level_name; }
	std::string get_current_level() const { return _current_level_name; }
	//Collision functions
	void find_which_enemy_to_damage(char* shape_1, char* shape_2);
	void find_which_enemy_is_in_range(char* visitor_shape, bool in_range);
	void find_which_enemy_has_the_player_in_range(b2ShapeId sensor_shape[], bool inrange);

	//Attack Functions
	void defeat_enemy(int which_enemy);
	void player_knockback(int enemy);
	void enemy_knockback(int index);
private:
	b2WorldId world_id;
	std::vector<b2BodyId> bodies;
	std::vector<std::shared_ptr<sf::RectangleShape>> sprites;
	std::shared_ptr<Entity> _player;
	std::vector<std::shared_ptr<Entity>> _walls;
	void _load_level(const std::string& file_path);
	bool _is_paused = false;
	std::vector<std::shared_ptr<Entity>> _enemies;
	int timer = 0;
	std::string _current_level_name;
	bool scene_restart;

	//User data for the bodies in the scene
	const void* player_user_data;
	const void* enemy_user_data;
	const void* fireball_user_data;
};

class MenuScene;
class SettingsScene;
class CreditsScene;

struct Scenes
{
	static std::shared_ptr<Scene> physics;
	static std::shared_ptr<MenuScene> menuScene;
	static std::shared_ptr<SettingsScene> settingsScene;
	static std::shared_ptr<CreditsScene> creditsScene;
	static std::shared_ptr<LevelScenes> levels;
};
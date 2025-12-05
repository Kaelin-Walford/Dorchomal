#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "physics.hpp"
#include "game_parameters.hpp"
#include "scenes.hpp"
#include "UI/menu_scene.hpp"
#include "UI/settings_scene.hpp"
#include "UI/game_settings.hpp"
#include "UI/credits_scene.hpp"

using param = Parameters;

int main()
{
	Physics::initialise();

	// Load saved settings
	GameSettings::get_instance().load_from_file();

	Scenes::physics = std::make_shared<PhysicsScene>();
	Scenes::physics->load();

	Scenes::kaelinsPlayground = std::make_shared<KaelinsPlayground>();
	Scenes::kaelinsPlayground->load();

	Scenes::menuScene = std::make_shared<MenuScene>();
	Scenes::menuScene->load();

	Scenes::settingsScene = std::make_shared<SettingsScene>();
	Scenes::settingsScene->load();

	Scenes::creditsScene = std::make_shared<CreditsScene>();
	Scenes::creditsScene->load();

	GameSystem::set_active_scene(Scenes::menuScene);

	GameSystem::start(param::game_width, param::game_height, "physics", param::time_step, true);

	Physics::shutdown();
	return 0;
}
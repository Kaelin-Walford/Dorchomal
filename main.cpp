#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "physics.hpp"
#include "game_parameters.hpp"
#include "scenes.hpp"

using param = Parameters;

int main()
{
	Physics::initialise();
	Scenes::physics = std::make_shared<PhysicsScene>();
	Scenes::physics->load();
	Scenes::kaelinsPlayground = std::make_shared<KaelinsPlayground>();
	Scenes::kaelinsPlayground->load();
	GameSystem::set_active_scene(Scenes::kaelinsPlayground);
	GameSystem::start(param::game_width, param::game_height, "physics", param::time_step, true);
	Physics::shutdown();
	return 0;
}

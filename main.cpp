#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "scenes.hpp"

using param = Parameters;

int main()
{
	Scenes::physics = std::make_shared<PhysicsScene>();
	Scenes::physics->load();
	GameSystem::set_active_scene(Scenes::physics);
	GameSystem::start(param::game_width, param::game_height, "physics", param::time_step);
	return 0;
}

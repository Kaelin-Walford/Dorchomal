#include <iostream>
#include "game_system.hpp"
#include "renderer.hpp"

std::shared_ptr<Scene> GameSystem::_active_scene;

void GameSystem::start(unsigned int width, unsigned int height,
	const std::string& name, const float& time_step)
{
	sf::RenderWindow window(sf::VideoMode({ width, height }), name);
	_init();
	Renderer::initialise(window);
	sf::Event event;
	while (window.isOpen())
	{
		static sf::Clock clock;
		float dt = clock.restart().asSeconds();
		while (window.pollEvent(event))
		{
			//Close the game if the x is pressed
			if (event.type == sf::Event::Closed)
			{
				window.close();
				clean();
				return;
			}
		}
		//close the game if escape is pressed
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}
		//Clear the window
		window.clear();

		//Prepare for new frame
		_update(dt);
		_render();
		sf::sleep(sf::seconds(time_step));
		//Wait for Vsync

		//Display next frame
		window.display();
	}
	window.close();
	clean();
}

void GameSystem::set_active_scene(const std::shared_ptr<Scene>& act_sc)
{
	_active_scene = act_sc;
}

void GameSystem::_init()
{

}

void GameSystem::clean()
{
	_active_scene->unload();
}

//Update the game objects
void GameSystem::_update(const float& dt)
{
	_active_scene->update(dt);
	Renderer::update(dt);
}

//Draw the game objects
void GameSystem::_render()
{
	_active_scene->render();
	Renderer::render();
}

//Update the game objects
void Scene::update(const float& dt)
{
	for (std::shared_ptr<Entity>& ent : _entities.list)
	{
		ent->update(dt);
	}
}

//Draw the game objects
void Scene::render()
{
	for (std::shared_ptr<Entity>& ent : _entities.list)
	{
		ent->render();
	}
}

void Scene::unload()
{
	_entities.list.clear();
}
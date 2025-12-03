#include <iostream>
#include "game_system.hpp"
#include "renderer.hpp"
#include "physics.hpp"
#include "../UI/menu_scene.hpp"
#include "../scenes.hpp"

std::shared_ptr<Scene> GameSystem::_active_scene;
bool GameSystem::_physics_enabled;
sf::Vector2i GameSystem::_mouse_position;

void GameSystem::start(unsigned int width, unsigned int height,
	const std::string& name, const float& time_step, bool physics_enabled)
{
	_physics_enabled = physics_enabled;
	sf::RenderWindow window(sf::VideoMode({ width, height }), name);
	_init();
	Renderer::initialise(window);
	//makes the mouse pointer invisible
	window.setMouseCursorVisible(false);
	sf::Event event;

	int timer = 0;

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

			if (Scenes::menuScene)
			{
				MenuState state = Scenes::menuScene->get_state();
				if (state == MenuState::MAIN_MENU || state == MenuState::PAUSED)
				{
					Scenes::menuScene->handle_event(event, window);
				}
			}

			if (Scenes::menuScene)
			{
				MenuState state = Scenes::menuScene->get_state();
				if (state == MenuState::MAIN_MENU || state == MenuState::PAUSED)
				{
					Scenes::menuScene->handle_event(event, window);
				}
			}
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			if (Scenes::menuScene && Scenes::menuScene->get_state() == MenuState::MAIN_MENU)
			{
				window.close();
			}
			// Escape during gameplay is handled in KaelinsPlayground::update()
		}

		window.clear();

		//Prepare for new frame
		_mouse_position = sf::Mouse::getPosition(window);
		_update(dt);
		_render();

		if (Scenes::menuScene)
		{
			MenuState state = Scenes::menuScene->get_state();
			if (state == MenuState::MAIN_MENU || state == MenuState::PAUSED)
			{
				Scenes::menuScene->render_buttons(window);
			}
		}

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
	if (_physics_enabled)
	{
		Physics::update(Physics::time_step);
	}
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
	_entities.update(dt);
	//for (std::shared_ptr<Entity>& ent : _entities.list)
	//{
		//ent->update(dt);
	//}
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

const std::shared_ptr<Entity>& Scene::make_entity()
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	_entities.list.push_back(entity);
	return _entities.list.back();
}
#include <iostream>
#include "game_system.hpp"
#include "renderer.hpp"
#include "physics.hpp"
#include "../UI/menu_scene.hpp"
#include "../UI/settings_scene.hpp"
#include "../scenes.hpp"
#include "audio_system.hpp"

std::shared_ptr<Scene> GameSystem::_active_scene;
sf::RenderWindow* GameSystem::_window = nullptr;
bool GameSystem::_physics_enabled;
sf::Vector2i GameSystem::_mouse_position;

void GameSystem::start(unsigned int width, unsigned int height,
	const std::string& name, const float& time_step, bool physics_enabled)
{
	_physics_enabled = physics_enabled;
	sf::RenderWindow window(sf::VideoMode({ width, height }), name);
	_window = &window;
	_init();
	Renderer::initialise(window);

	sf::Event event;

	int timer = 0;

	AudioSystem::load_music("The tomb of the last dragon.ogg");


	AudioSystem tem;
	tem.add_sound("dash.wav");
	//tem.play_sound();
	AudioSystem sys;
	sys.add_sound("Slash.wav");
	//sys.play_sound();
	AudioSystem was;
	was.add_sound("A tavern, a bard, a quest.wav");
	//was.play_sound();

	/*adds new buffer to the end of the sound buffer
	sf::SoundBuffer buffer;
	char path[100] = "../../../../resources/Sounds/Sound Effects/dash.wav";
	buffer.loadFromFile(path);


	//Adds the buffer sound to the end of the sound
	sf::Sound sound;
	sound.setBuffer(buffer);

	sound.play();*/

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

			// Handle menu scene events
			if (Scenes::menuScene)
			{
				MenuState state = Scenes::menuScene->get_state();
				if (state == MenuState::MAIN_MENU || state == MenuState::PAUSED)
				{
					Scenes::menuScene->handle_event(event, window);
					change_mouse_visibility(true, &window);
					if (AudioSystem::is_music_playing() == sf::SoundSource::Status::Playing)
					{
						AudioSystem::play_pause_music(false);
					}
				}
			}

			// Handle settings scene events
			if (Scenes::settingsScene && _active_scene == Scenes::settingsScene)
			{
				change_mouse_visibility(true, &window);
				Scenes::settingsScene->handle_event(event, window);
				if (AudioSystem::is_music_playing() == sf::SoundSource::Status::Playing)
				{
					AudioSystem::play_pause_music(false);
				}
			}

			//If the _active_scene is the game level so to hide the mouse pointer
			if (_active_scene == Scenes::levels)
			{
				change_mouse_visibility(false, &window);
				if (AudioSystem::is_music_playing() != sf::SoundSource::Status::Playing)
				{
					AudioSystem::play_pause_music(true);
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

		// Render menu buttons if menu is active
		if (Scenes::menuScene)
		{
			MenuState state = Scenes::menuScene->get_state();
			if (state == MenuState::MAIN_MENU || state == MenuState::PAUSED)
			{
				Scenes::menuScene->render_buttons(window);
			}
		}

		// Render settings UI if settings scene is active
		if (Scenes::settingsScene && _active_scene == Scenes::settingsScene)
		{
			Scenes::settingsScene->render_ui(window);
		}

		sf::sleep(sf::seconds(time_step));
		//Wait for Vsync

		//Display next frame
		window.display();
	}
	window.close();
	clean();
}

void GameSystem::change_mouse_visibility(bool visible, sf::RenderWindow* window)
{
	//makes the mouse pointer invisible
	window->setMouseCursorVisible(visible);
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

	// test physics dont update when game is paused
	if (_physics_enabled)
	{
		bool is_paused = false;
		if (Scenes::kaelinsPlayground && _active_scene == Scenes::kaelinsPlayground)
		{
			is_paused = Scenes::kaelinsPlayground->is_paused();
		}

		if (!is_paused)
		{
			Physics::update(Physics::time_step);
		}
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
sf::RenderWindow& GameSystem::get_window()
{
	return *_window;
}
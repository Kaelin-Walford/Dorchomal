#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "scenes.hpp"
#include "game_parameters.hpp"
#include "renderer.hpp"
#include "b2_utils.hpp"
#include "graphics_cmps.hpp"
#include "physics_cmps.hpp"
#include "physics.hpp"

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
	/*
	//creates the box2D world
	b2WorldDef world_def = b2DefaultWorldDef();
	//sets the gravity for the world
	world_def.gravity = b2Vec2({ 0.0f, param::g });
	//identifies the physics simulation
	world_id = b2CreateWorld(&world_def);
	*/

	
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
		b2BodyId b = b2::create_physics_box(Physics::get_world_id(), false, s);
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
	cmp->create_capsule_shape(sf::Vector2f(param::player_size[0], param::player_size[1]), param::player_weight, param::player_friction, param::player_restitution);
}

void KaelinsPlayground::update(const float &dt)
{
	Scene::update(dt);
	_entities.update(dt);
	//std::cout << _entities[0];
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
	_player.reset();
}
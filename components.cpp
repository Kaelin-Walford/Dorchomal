#include "components.hpp"
#include "game_parameters.hpp"
#include "level_system.hpp"
#include "renderer.hpp"
#include "game_system.hpp"
#include "scenes.hpp"
#include <iostream>
#include <cmath>

//level_system is not proprammed yet
//using ls = LevelSystem;
using param = Parameters;
using gs = GameSystem;

/*
//Shape components
//Update keeps the SFML shape at the same position as the entity
void ShapeComponent::update(const float& dt)
{
	_shape->setPosition(_parent->get_position());
}

//Adds the shape to the renderer queue
void ShapeComponent::render() { Renderer::queue(_shape.get()); }

sf::Shape& ShapeComponent::get_shape() const { return *_shape; }

//Links to the parent entity p - defaults to a circle shape
ShapeComponent::ShapeComponent(Entity* p) : Component(p), _shape(std::make_shared<sf::CircleShape>()) {}
*/

//Actor movement component
void ActorMovementComponent::update(const float& dt) {}

//This sets the default speed to equal 100
ActorMovementComponent::ActorMovementComponent(Entity* p)
	: _speed(100.0f), Component(p)
{
}

//Checks that the new position is not inside a wall
//Curently commented out as it requires level_system to be programed
/*
bool ActorMovementComponent::_valid_move(const sf::Vector2f& pos)
{
	return (ls::get_tile_at(pos) != ls::WALL);
}
*/

//moves the player by taking in a vector2f
void ActorMovementComponent::move(const sf::Vector2f& p)
{
	sf::Vector2f new_pos = _parent->get_position() + p;
	if (/*_valid_move(new_pos)*/true)
	{
		_parent->set_position(new_pos);
	}
}

//moves the player by x,y
void ActorMovementComponent::move(float x, float y)
{
	move(sf::Vector2f(x, y));
}

float ActorMovementComponent::get_speed() const
{
	return _speed;
}

void ActorMovementComponent::set_speed(float speed)
{ 
	_speed = speed;
}

//Player movement component
PlayerMovementComponent::PlayerMovementComponent(Entity* p) : ActorMovementComponent(p){}

//Adds controls to the movenemt component
void PlayerMovementComponent::update(const float& dt)
{
	sf::Vector2f new_pos(_parent->get_position().x, _parent->get_position().y);

	//Move up
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		move(0, -_speed * dt);

	//Move down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		move(0, _speed * dt);

	//Move left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		move(-_speed * dt, 0);

	//Move Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		move(_speed * dt, 0);

}

//Enemy AI Component
//AI component that implements a simple roaming function - will need to be changed to work with our game
const sf::Vector2i EnemyAIComponent::directions[] = { {1, 0}, {0, 1}, {0, -1}, {-1, 0} };

EnemyAIComponent::EnemyAIComponent(Entity* p)
	: ActorMovementComponent(p)
{
	_direction = sf::Vector2f(directions[(rand() % 4)]);
}

//Enemy update also requires level_system.cpp to be programed
/*
void EnemyAIComponent::update(const float& dt)
{
	//amount to move
	const float mva = static_cast<float>(dt * _speed);
	//Curent position
	const sf::Vector2f pos = _parent->get_position();
	//Next position
	const sf::Vector2f newpos = pos + _direction * mva;
	//Inverse of our current direction
	const sf::Vector2i baddir = -1 * sf::Vector2i(_direction);
	//Random new direction
	sf::Vector2i newdir = directions[(rand() % 4)];
	ls::Tile current_tile = ls::get_tile_at(pos);
	switch (_state)
	{
	case ROAMING:
		if (ls::get_tile_at(newpos) == ls::WALL || current_tile == ls::WAYPOINT)// Wall in front or at waypoint
		{
			_state = ROTATING; // start rotate
		}
		else
		{
			move(mva * _direction); //keep moving
		}
		break;

	case ROTATING:
		while (
			// Don't reverse
			(newdir == baddir)
			// and Don't pick a direction that will lead to a wall
			&& ls::get_tile_at(pos + sf::Vector2f(newdir) * mva) == ls::WALL
			)
		{
			newdir = directions[(rand() % 4)];
			// pick new direction
		}
		_direction = sf::Vector2f(newdir);
		_state = ROTATED;
		break;

	case ROTATED:
		//have we left the waypoint?
		if (ls::get_tile_at(pos) != ls::WAYPOINT)
		{
			_state = ROAMING; //yes
		}
		if (ls::get_tile_at(newpos) == ls::WALL)//if there is a wall in front rotate again
			_state = ROTATING;
		move(_direction * mva); //No
		break;
	}
	ActorMovementComponent::update(dt);
}
*/

//Pickup Component
//makes an entity a collectible item - will need to be modified for our game
PickupComponent::PickupComponent(Entity* p, bool big) : Component(p), _big(big) {}

void PickupComponent::update(const float&)
{
	auto vect_distance = [](sf::Vector2f a, sf::Vector2f b) -> float
		{
			return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
		};
	for (std::shared_ptr<Entity> e : gs::get_active_scene()->get_entities())
	{       //every entity in the scene
		if (vect_distance(_parent->get_position(), e->get_position()) < 30.f)
		{      //within 30.f unit of me
			std::vector<std::shared_ptr<ActorMovementComponent>> comp =
				e->get_compatible_components<ActorMovementComponent>();               //get the entity ActorMovementComponent, if it has one
			if (!comp.empty())
			{        //if it has one
// nom nom
				if (_big)
					comp[0]->set_speed(comp[0]->get_speed() + param::big_speed_up);                      //speed the entity up
				else
					comp[0]->set_speed(comp[0]->get_speed() + param::small_speed_up);
				_parent->set_for_delete(); //delete myself
				break;                   //stop looking
			}
		}
	}
}
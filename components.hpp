#pragma once
#include "ecm.hpp"

//Draws an SFML shape and keeps it positioned with its parent entity
class ShapeComponent : public Component
{
public:
	ShapeComponent() = delete;
	explicit ShapeComponent(Entity* const p);

	void update(const float& dt) override;
	void render() override;

	sf::Shape& get_shape() const;

	//Allows for different shape types
	template <typename T, typename... Targs>
	void set_shape(Targs... params)
	{
		_shape.reset(new T(params...));
	}
protected:
	std::shared_ptr<sf::Shape> _shape;
};

//Adds movement logic to an entity
class ActorMovementComponent : public Component
{
public:
	explicit ActorMovementComponent(Entity* p);
	ActorMovementComponent() = delete;

	float get_speed() const;
	void set_speed(float _speed);

	//moves the entity by x,y
	void move(const sf::Vector2f&);
	void move(float x, float y);

	void render() override {}
	void update(const float& dt) override;
protected:
	//bool _valid_move(const sf::Vector2f&);
	float _speed;
};

class PlayerMovementComponent : public ActorMovementComponent
{
public:
	PlayerMovementComponent() = delete;
	PlayerMovementComponent(Entity* p);

	void update(const float& dt) override;
};

class EnemyAIComponent : public ActorMovementComponent
{
protected:
	static const sf::Vector2i directions[];
	sf::Vector2f _direction;
	enum state { ROAMING, ROTATING, ROTATED };
	state _state = ROAMING;
public:
	EnemyAIComponent() = delete;
	EnemyAIComponent(Entity* p);
	//void update(const float& dt) override;
};

class PickupComponent : public Component
{
protected:
	bool _big;

public:
	explicit PickupComponent(Entity* p, bool big);
	PickupComponent() = delete;

	void render() override {}
	void update(const float& dt) override;
};
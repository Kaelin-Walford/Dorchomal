#pragma once
#include "game_system.hpp"
#include "box2d/box2d.h"

class PhysicsScene : public Scene
{
private:
	b2WorldId world_id;
	std::vector<b2BodyId> bodies;
	std::vector<std::shared_ptr<sf::RectangleShape>> sprites;
public:
	PhysicsScene() = default;
	void update(const float& dt) override;
	void render() override;
	void load()override;
	void unload() override;
};

struct Scenes
{
	static std::shared_ptr<Scene> physics;
};
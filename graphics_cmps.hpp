#pragma once
#include "ecm.hpp"
#include <SFML/Graphics.hpp>

//Draws an SFML shape and keeps it positioned with its parent entity
class ShapeComponent : public Component
{
public:
	ShapeComponent() = delete;
	explicit ShapeComponent(Entity* const p);

	void update(const float& dt) override;
	void render() override;

	sf::Shape& get_shape() const;

	template <typename T, typename... Targs>
	void set_shape(Targs... params)
	{
		_shape = std::make_shared<T>(params...);
	}
protected:
	std::shared_ptr<sf::Shape> _shape;
};


class SpriteComponent : public Component
{
protected:
	std::shared_ptr<sf::Sprite> _sprite;
	std::shared_ptr<sf::Texture> _texture;
public:
	SpriteComponent() = delete;

	explicit SpriteComponent(Entity* p);
	void update(const float& dt) override;
	void render() override;

	sf::Sprite& get_sprite() const;


	void set_texure(std::shared_ptr<sf::Texture> tex);
};
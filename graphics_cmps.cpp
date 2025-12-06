#include "game_parameters.hpp"
#include "graphics_cmps.hpp"
#include "renderer.hpp"

void ShapeComponent::update(const float& dt)
{
	_shape->setPosition(_parent->get_position());
	_shape->setRotation(_parent->get_rotation());
}

void ShapeComponent::render() { Renderer::queue(_shape.get()); }

sf::Shape& ShapeComponent::get_shape() const { return *_shape; }

ShapeComponent::ShapeComponent(Entity* p) : Component(p), _shape(std::make_shared<sf::CircleShape>()) {}


void SpriteComponent::set_texure(std::shared_ptr<sf::Texture> tex)
{
	_texture = tex;
	_sprite->setTexture(*_texture);
}
sf::Sprite& SpriteComponent::get_sprite() const { return *_sprite; }


SpriteComponent::SpriteComponent(Entity* p)
	: Component(p), _sprite(std::make_shared<sf::Sprite>())
{
}

void SpriteComponent::update(const float& dt)
{
	_sprite->setPosition(_parent->get_position());
	_sprite->setRotation((_parent->get_rotation()) / M_PI * 180.f);
}

void SpriteComponent::render() { Renderer::queue(_sprite.get()); }
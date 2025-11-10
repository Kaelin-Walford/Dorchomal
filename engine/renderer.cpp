#include "renderer.hpp"
#include <queue>

static std::queue<const sf::Drawable*> sprites;
static sf::RenderWindow* rw;

void Renderer::initialise(sf::RenderWindow& r)
{
	rw = &r;
}

sf::RenderWindow& Renderer::get_window()
{
	return *rw;
}

//removes the sprites from the render queue
void Renderer::shutdown()
{
	while (!sprites.empty())
		sprites.pop();
}

void Renderer::update(const float&) {}

//renders the sprites then removes them from the queue
void Renderer::render()
{
	if (rw == nullptr)
	{
		throw("No render window set! ");
	}
	while (!sprites.empty())
	{
		rw->draw(*sprites.front());
		sprites.pop();
	}
}

//adds a sprite to the queue
// - SFML renders each sprite on top of all previous rendered ones, so make sure each sprite is added in the correct order background first - foreground last.
void Renderer::queue(const sf::Drawable* s)
{
	sprites.push(s);
}
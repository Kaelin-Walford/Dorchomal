#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "../engine/audio_system.hpp"

class UIButton
{
public:
    UIButton(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& text, sf::Font* font);

    void update(const sf::Vector2i& mousePos);
    void render(sf::RenderWindow& window);
    bool is_clicked(const sf::Event& event, const sf::Vector2i& mousePos);

    void set_callback(std::function<void()> callback);
    void set_colors(const sf::Color& normal, const sf::Color& hover, const sf::Color& text);

private:
    sf::RectangleShape _shape;
    sf::Text _text;
    sf::Font* _font;

    sf::Color _normal_color;
    sf::Color _hover_color;
    sf::Color _text_color;

    bool _is_hovered;
    std::function<void()> _callback;

    //Sounds
    AudioSystem _hover_sound;
    AudioSystem _click_sound;
    bool _already_played_sound;
};
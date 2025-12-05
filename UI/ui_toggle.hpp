#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

// UI Toggle for on/off settings
class UIToggle
{
public:
    UIToggle(const sf::Vector2f& position, const std::string& label,
        bool initial_state, sf::Font* font);

    void update(const sf::Vector2i& mousePos);
    void render(sf::RenderWindow& window);
    bool is_clicked(const sf::Event& event, const sf::Vector2i& mousePos);

    bool get_state() const { return _is_on; }
    void set_state(bool state);

    void set_callback(std::function<void(bool)> callback);

private:
    // Visual components
    sf::RectangleShape _background;
    sf::CircleShape _knob;
    sf::Text _label_text;
    sf::Text _state_text;

    // Data
    sf::Vector2f _position;
    bool _is_on;
    bool _is_hovered;

    // Callback
    std::function<void(bool)> _callback;

    // Helper
    void update_visuals();
};
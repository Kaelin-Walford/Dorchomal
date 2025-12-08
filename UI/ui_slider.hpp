#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "../engine/audio_system.hpp"


class UISlider
{
public:
    UISlider(const sf::Vector2f& position, float width,
        float min_value, float max_value, float current_value,
        const std::string& label, sf::Font* font, sf::Font* number_font = nullptr);

    void update(const sf::Vector2i& mousePos, bool mouse_pressed);
    void render(sf::RenderWindow& window);

    float get_value() const { return _current_value; }
    void set_value(float value);

    void set_callback(std::function<void(float)> callback);

private:
    // Visual components
    sf::RectangleShape _track;        // Background track
    sf::RectangleShape _fill;         // Filled portion
    sf::CircleShape _handle;          // Draggable handle
    sf::Text _label_text;             // Label
    sf::Text _value_text;             // Current value 

    // Data
    float _min_value;
    float _max_value;
    float _current_value;
    float _width;
    sf::Vector2f _position;

    // State
    bool _is_dragging;
    bool _is_hovered;

    // Callback
    std::function<void(float)> _callback;

    // Helper
    void update_visuals();
    float get_percentage() const;

    //Sounds
    AudioSystem _hover_sound;
    AudioSystem _click_sound;
    bool _already_played_sound;
};
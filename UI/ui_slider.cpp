#include "ui_slider.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

UISlider::UISlider(const sf::Vector2f& position, float width,
    float min_value, float max_value, float current_value,
    const std::string& label, sf::Font* font)
    : _position(position), _width(width), _min_value(min_value),
    _max_value(max_value), _current_value(current_value),
    _is_dragging(false), _is_hovered(false)
{
    // Setup track
    _track.setSize(sf::Vector2f(width, 8.f));
    _track.setPosition(position.x, position.y + 30.f);
    _track.setFillColor(sf::Color(60, 60, 60));
    _track.setOutlineColor(sf::Color(40, 40, 40));
    _track.setOutlineThickness(2.f);

    // Setup fill
    _fill.setSize(sf::Vector2f(0, 8.f));
    _fill.setPosition(position.x, position.y + 30.f);
    _fill.setFillColor(sf::Color(100, 150, 255));

    // Setup handle
    _handle.setRadius(12.f);
    _handle.setOrigin(12.f, 12.f);
    _handle.setFillColor(sf::Color(150, 200, 255));
    _handle.setOutlineColor(sf::Color::White);
    _handle.setOutlineThickness(2.f);

    // Setup label
    _label_text.setFont(*font);
    _label_text.setString(label);
    _label_text.setCharacterSize(20);
    _label_text.setFillColor(sf::Color::White);
    _label_text.setPosition(position.x, position.y);

    // Setup value text
    _value_text.setFont(*font);
    _value_text.setCharacterSize(18);
    _value_text.setFillColor(sf::Color(200, 200, 200));

    update_visuals();
}

void UISlider::set_value(float value)
{
    _current_value = std::max(_min_value, std::min(_max_value, value));
    update_visuals();
}

void UISlider::set_callback(std::function<void(float)> callback)
{
    _callback = callback;
}

float UISlider::get_percentage() const
{
    if (_max_value == _min_value) return 0.f;
    return (_current_value - _min_value) / (_max_value - _min_value);
}

void UISlider::update_visuals()
{
    float percentage = get_percentage();

    // Update fill
    _fill.setSize(sf::Vector2f(_width * percentage, 8.f));

    // Update handle position
    float handle_x = _position.x + (_width * percentage);
    float handle_y = _position.y + 34.f;
    _handle.setPosition(handle_x, handle_y);

    // Update value text
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << _current_value;
    _value_text.setString(ss.str());

    // Position value text to the right
    sf::FloatRect textBounds = _value_text.getLocalBounds();
    _value_text.setPosition(_position.x + _width + 15.f, _position.y + 24.f);
}

void UISlider::update(const sf::Vector2i& mousePos, bool mouse_pressed)
{
    // Check if mouse is over handle
    sf::Vector2f handle_pos = _handle.getPosition();
    float dx = mousePos.x - handle_pos.x;
    float dy = mousePos.y - handle_pos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    _is_hovered = (distance <= 12.f);

    // Start dragging
    if (_is_hovered && mouse_pressed && !_is_dragging)
    {
        _is_dragging = true;
    }

    // Stop dragging
    if (!mouse_pressed)
    {
        _is_dragging = false;
    }

    // Update value while dragging
    if (_is_dragging)
    {
        float track_x = mousePos.x - _position.x;
        track_x = std::max(0.f, std::min(_width, track_x));

        float percentage = track_x / _width;
        float new_value = _min_value + (percentage * (_max_value - _min_value));

        if (new_value != _current_value)
        {
            _current_value = new_value;
            update_visuals();

            if (_callback)
            {
                _callback(_current_value);
            }
        }
    }

    // Visual feedback
    if (_is_hovered || _is_dragging)
    {
        _handle.setFillColor(sf::Color(200, 230, 255));
        _handle.setOutlineThickness(3.f);
    }
    else
    {
        _handle.setFillColor(sf::Color(150, 200, 255));
        _handle.setOutlineThickness(2.f);
    }
}

void UISlider::render(sf::RenderWindow& window)
{
    window.draw(_label_text);
    window.draw(_track);
    window.draw(_fill);
    window.draw(_handle);
    window.draw(_value_text);
}
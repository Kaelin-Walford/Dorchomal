#include "ui_toggle.hpp"

UIToggle::UIToggle(const sf::Vector2f& position, const std::string& label,
    bool initial_state, sf::Font* font)
    : _position(position), _is_on(initial_state), _is_hovered(false)
{
    // Setup background 
    _background.setSize(sf::Vector2f(60.f, 30.f));
    _background.setPosition(position.x, position.y + 25.f);
    _background.setOutlineThickness(2.f);

    // Setup slider
    _knob.setRadius(12.f);
    _knob.setOrigin(12.f, 12.f);
    _knob.setFillColor(sf::Color::White);

    // Setup label
    _label_text.setFont(*font);
    _label_text.setString(label);
    _label_text.setCharacterSize(20);
    _label_text.setFillColor(sf::Color::White);
    _label_text.setPosition(position.x, position.y);

    // Setup state text
    _state_text.setFont(*font);
    _state_text.setCharacterSize(18);
    _state_text.setFillColor(sf::Color(200, 200, 200));
    _state_text.setPosition(position.x + 75.f, position.y + 28.f);

    update_visuals();
}

void UIToggle::set_state(bool state)
{
    _is_on = state;
    update_visuals();
}

void UIToggle::set_callback(std::function<void(bool)> callback)
{
    _callback = callback;
}

void UIToggle::update_visuals()
{
    if (_is_on)
    {
        
        _background.setFillColor(sf::Color(50, 150, 50));
        _background.setOutlineColor(sf::Color(70, 200, 70));
        _knob.setPosition(_position.x + 45.f, _position.y + 40.f);
        _state_text.setString("ON");
        _state_text.setFillColor(sf::Color(100, 255, 100));
    }
    else
    {
        
        _background.setFillColor(sf::Color(60, 60, 60));
        _background.setOutlineColor(sf::Color(40, 40, 40));
        _knob.setPosition(_position.x + 15.f, _position.y + 40.f);
        _state_text.setString("OFF");
        _state_text.setFillColor(sf::Color(150, 150, 150));
    }
}

void UIToggle::update(const sf::Vector2i& mousePos)
{
    sf::FloatRect bounds = _background.getGlobalBounds();
    _is_hovered = bounds.contains(static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y));

    
    if (_is_hovered)
    {
        _background.setOutlineThickness(3.f);
        _knob.setScale(1.1f, 1.1f);
    }
    else
    {
        _background.setOutlineThickness(2.f);
        _knob.setScale(1.f, 1.f);
    }
}

bool UIToggle::is_clicked(const sf::Event& event, const sf::Vector2i& mousePos)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        if (_is_hovered)
        {
            _is_on = !_is_on;
            update_visuals();

            if (_callback)
            {
                _callback(_is_on);
            }

            return true;
        }
    }
    return false;
}

void UIToggle::render(sf::RenderWindow& window)
{
    window.draw(_label_text);
    window.draw(_background);
    window.draw(_knob);
    window.draw(_state_text);
}
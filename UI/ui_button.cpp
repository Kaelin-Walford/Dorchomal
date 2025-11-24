#include "ui_button.hpp"

UIButton::UIButton(const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& text, sf::Font* font)
    : _font(font), _is_hovered(false)
{
    // Setup button shape
    _shape.setPosition(position);
    _shape.setSize(size);
    _shape.setOrigin(size.x / 2.f, size.y / 2.f);

    // Default colors
    _normal_color = sf::Color(70, 70, 70, 255);
    _hover_color = sf::Color(100, 100, 100, 255);
    _text_color = sf::Color::White;

    _shape.setFillColor(_normal_color);
    _shape.setOutlineColor(sf::Color(200, 200, 200, 255));
    _shape.setOutlineThickness(2.f);

    // Setup text
    if (_font)
    {
        _text.setFont(*_font);
        _text.setString(text);
        _text.setCharacterSize(24);
        _text.setFillColor(_text_color);

        // Center text in button
        sf::FloatRect textBounds = _text.getLocalBounds();
        _text.setOrigin(textBounds.left + textBounds.width / 2.f,
            textBounds.top + textBounds.height / 2.f);
        _text.setPosition(position);
    }
}

void UIButton::update(const sf::Vector2i& mousePos)
{
    sf::FloatRect bounds = _shape.getGlobalBounds();
    _is_hovered = bounds.contains(static_cast<sf::Vector2f>(mousePos));

    _shape.setFillColor(_is_hovered ? _hover_color : _normal_color);
}

void UIButton::render(sf::RenderWindow& window)
{
    window.draw(_shape);
    window.draw(_text);
}

bool UIButton::is_clicked(const sf::Event& event, const sf::Vector2i& mousePos)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::FloatRect bounds = _shape.getGlobalBounds();
        if (bounds.contains(static_cast<sf::Vector2f>(mousePos)))
        {
            if (_callback)
            {
                _callback();
            }
            return true;
        }
    }
    return false;
}

void UIButton::set_callback(std::function<void()> callback)
{
    _callback = callback;
}

void UIButton::set_colors(const sf::Color& normal, const sf::Color& hover, const sf::Color& text)
{
    _normal_color = normal;
    _hover_color = hover;
    _text_color = text;
    _text.setFillColor(text);
    _shape.setFillColor(_is_hovered ? _hover_color : _normal_color);
}
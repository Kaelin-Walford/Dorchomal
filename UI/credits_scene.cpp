#include "credits_scene.hpp"
#include "menu_scene.hpp"
#include "ui_button.hpp"
#include "../game_parameters.hpp"
#include "../engine/renderer.hpp"
#include "../scenes.hpp"
#include <iostream>

using param = Parameters;

void CreditsScene::load()
{
    if (!_font.loadFromFile("resources/fonts/pixelated.otf"))
    {
        std::cerr << "Failed to load font for credits. Using default font." << std::endl;
    }

    if (!_number_font.loadFromFile("resources/fonts/CreditCard-26Me.ttf"))
    {
        std::cerr << "Failed to load number font. Using default font." << std::endl;
    }

    _overlay.setSize(sf::Vector2f(param::game_width, param::game_height));
    _overlay.setFillColor(sf::Color(0, 0, 0, 240));

    _scroll_position = param::game_height;
    _scroll_speed = 50.0f;
    _auto_scroll = true;

    create_credits_text();
    create_back_button();
}

void CreditsScene::create_credits_text()
{
    _credits_lines.clear();
    _base_positions.clear();

    float centerX = param::game_width / 2.0f;
    float currentY = 0.0f;

    auto add_line = [&](const std::string& text, unsigned int size, const sf::Color& color, float spacing = 50.0f, bool useNumberFont = false) {
        sf::Text line;
        line.setFont(useNumberFont ? _number_font : _font);
        line.setString(text);
        line.setCharacterSize(size);
        line.setFillColor(color);

        sf::FloatRect bounds = line.getLocalBounds();
        line.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
        line.setPosition(centerX, currentY);

        _credits_lines.push_back(line);
        _base_positions.push_back(currentY);
        currentY += spacing;
        };

    add_line("DORCHOMAL", 72, sf::Color::White, 100);
    add_line("A Game by KMS", 48, sf::Color(200, 200, 255), 150);
    add_line("GAME DEVELOPMENT", 36, sf::Color::Yellow, 80);
    add_line("Developers", 24, sf::Color::White, 60);
    add_line("40650381, 40664564, 40652155", 22, sf::Color(150, 255, 150), 100, true);
    add_line("Game Audio", 24, sf::Color::White, 60);
    add_line("40650381", 22, sf::Color(150, 255, 150), 100, true);
    add_line("Game Level Design", 24, sf::Color::White, 60);
    add_line("40652155", 22, sf::Color(150, 255, 150), 150, true);
    add_line("TECHNICAL", 36, sf::Color::Yellow, 80);
    add_line("Engine Architecture", 24, sf::Color::White, 60);
    add_line("40664564", 22, sf::Color(150, 255, 150), 50, true);
    add_line("Entity Component System", 20, sf::Color(200, 200, 200), 50);
    add_line("Custom Physics Integration", 20, sf::Color(200, 200, 200), 100);
    add_line("UI System Design", 24, sf::Color::White, 60);
    add_line("40664564", 22, sf::Color(150, 255, 150), 50, true);
    add_line("Menu System", 20, sf::Color(200, 200, 200), 50);
    add_line("Settings Management", 20, sf::Color(200, 200, 200), 100);
    add_line("Audio Controls", 24, sf::Color::White, 60);
    add_line("40650381, 40664564", 22, sf::Color(150, 255, 150), 150, true);
    add_line("BUILT WITH", 36, sf::Color::Yellow, 80);
    add_line("SFML 2.6.2", 28, sf::Color::Cyan, 50);
    add_line("Simple and Fast Multimedia Library", 20, sf::Color(150, 200, 255), 90);
    add_line("Box2D 3.0", 28, sf::Color::Cyan, 50);
    add_line("2D Physics Engine", 20, sf::Color(150, 200, 255), 90);
    add_line("C++", 28, sf::Color::Cyan, 50);
    add_line("Programming Language", 20, sf::Color(150, 200, 255), 90);
    add_line("CMake", 28, sf::Color::Cyan, 50);
    add_line("Build System", 20, sf::Color(150, 200, 255), 90);
    add_line("Visual Studio", 28, sf::Color::Cyan, 50);
    add_line("Development Environment", 20, sf::Color(150, 200, 255), 150);
    add_line("SPECIAL THANKS", 36, sf::Color::Yellow, 80);
    add_line("To everyone who played this game", 24, sf::Color::White, 100);
    add_line("Created for SET09121", 24, sf::Color(200, 200, 200), 50);
    add_line("Games Engineering", 20, sf::Color(150, 150, 150), 150);
    add_line("COPYRIGHT & LICENSE", 36, sf::Color::Yellow, 80);
    add_line("(C) 2024 KMS", 20, sf::Color(150, 150, 150), 50);
    add_line("GPL-3.0 License", 20, sf::Color(150, 150, 150), 50);
    add_line("All Rights Reserved", 20, sf::Color(150, 150, 150), 150);
    add_line("THANK YOU FOR PLAYING!", 48, sf::Color::White, 200);
}

void CreditsScene::create_back_button()
{
    float centerX = param::game_width / 2.0f;
    float buttonY = param::game_height - 100.0f;

    _back_button = std::make_unique<UIButton>(
        sf::Vector2f(centerX, buttonY),
        sf::Vector2f(300.0f, 60.0f),
        "Back",
        &_font
    );

    _back_button->set_callback([this]() {
        if (_return_scene) {
            if (Scenes::menuScene) {
                Scenes::menuScene->show_main_menu();
            }
            GameSystem::set_active_scene(_return_scene);
        }
        });
}

void CreditsScene::update(const float& dt)
{
    Scene::update(dt);

    if (_auto_scroll)
    {
        _scroll_position -= _scroll_speed * dt;

        if (!_credits_lines.empty() && !_base_positions.empty())
        {
            float lastY = _base_positions.back() + _scroll_position;
            if (lastY < -200.0f)
            {
                _scroll_position = param::game_height;
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        _scroll_position += _scroll_speed * 2.0f * dt;
        _auto_scroll = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _scroll_position -= _scroll_speed * 2.0f * dt;
        _auto_scroll = false;
    }

    static bool spacePressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        if (!spacePressed)
        {
            _auto_scroll = !_auto_scroll;
            spacePressed = true;
        }
    }
    else
    {
        spacePressed = false;
    }

    for (size_t i = 0; i < _credits_lines.size(); ++i)
    {
        _credits_lines[i].setPosition(_credits_lines[i].getPosition().x, _base_positions[i] + _scroll_position);
    }
}

void CreditsScene::render()
{
    Renderer::queue(&_overlay);

    for (auto& line : _credits_lines)
    {
        float y = line.getPosition().y;
        if (y >= -100 && y <= param::game_height + 100)
        {
            Renderer::queue(&line);
        }
    }
}

void CreditsScene::handle_event(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            if (_return_scene) {
                if (Scenes::menuScene) {
                    Scenes::menuScene->show_main_menu();
                }
                GameSystem::set_active_scene(_return_scene);
            }
        }
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    if (_back_button)
    {
        _back_button->update(mousePos);
        _back_button->render(window);
        _back_button->is_clicked(event, mousePos);
    }
}

void CreditsScene::unload()
{
    _credits_lines.clear();
    _base_positions.clear();
    _back_button.reset();
}
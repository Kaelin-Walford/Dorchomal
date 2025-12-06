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
    // Load font
    if (!_font.loadFromFile("resources/fonts/pixelated.otf"))
    {
        std::cerr << "Failed to load font for credits. Using default font." << std::endl;
    }

    // Setup overlay
    _overlay.setSize(sf::Vector2f(param::game_width, param::game_height));
    _overlay.setFillColor(sf::Color(0, 0, 0, 240));

    // Initialize scroll settings
    _scroll_position = param::game_height;
    _scroll_speed = 50.0f;
    _auto_scroll = true;

    // Create credits content
    create_credits_text();

    // Create back button
    create_back_button();
}

void CreditsScene::create_credits_text()
{
    _credits_lines.clear();

    float centerX = param::game_width / 2.0f;
    float lineHeight = 50.0f;
    float currentY = 0.0f;

    // Helper lambda to add text
    auto add_line = [&](const std::string& text, unsigned int size, const sf::Color& color) {
        sf::Text line;
        line.setFont(_font);
        line.setString(text);
        line.setCharacterSize(size);
        line.setFillColor(color);

        // Center the text
        sf::FloatRect bounds = line.getLocalBounds();
        line.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
        line.setPosition(centerX, currentY);

        _credits_lines.push_back(line);
        currentY += lineHeight;
        };

    auto add_space = [&](float space) {
        currentY += space;
        };

    // === CREDITS CONTENT ===

    add_space(100);

    add_line("DORCHOMAL", 72, sf::Color::White);
    add_space(50);

    add_line("A Game by Scott", 48, sf::Color(200, 200, 255));
    add_space(100);

    // Game Development Section
    add_line("GAME DEVELOPMENT", 36, sf::Color::Yellow);
    add_space(30);
    add_line("Lead Developer", 24, sf::Color::White);
    add_line("Scott", 28, sf::Color(150, 255, 150));
    add_space(50);

    add_line("Programming", 24, sf::Color::White);
    add_line("Scott", 28, sf::Color(150, 255, 150));
    add_space(50);

    add_line("Game Design", 24, sf::Color::White);
    add_line("Scott", 28, sf::Color(150, 255, 150));
    add_space(100);

    // Technical Section
    add_line("TECHNICAL", 36, sf::Color::Yellow);
    add_space(30);
    add_line("Engine Architecture", 24, sf::Color::White);
    add_line("Entity Component System", 20, sf::Color(200, 200, 200));
    add_line("Custom Physics Integration", 20, sf::Color(200, 200, 200));
    add_space(50);

    add_line("UI System Design", 24, sf::Color::White);
    add_line("Menu System", 20, sf::Color(200, 200, 200));
    add_line("Settings Management", 20, sf::Color(200, 200, 200));
    add_line("Audio Controls", 20, sf::Color(200, 200, 200));
    add_space(100);

    // Libraries & Tools Section
    add_line("BUILT WITH", 36, sf::Color::Yellow);
    add_space(30);

    add_line("SFML 2.6", 28, sf::Color::Cyan);
    add_line("Simple and Fast Multimedia Library", 20, sf::Color(150, 200, 255));
    add_space(40);

    add_line("Box2D 3.0", 28, sf::Color::Cyan);
    add_line("2D Physics Engine", 20, sf::Color(150, 200, 255));
    add_space(40);

    add_line("C++", 28, sf::Color::Cyan);
    add_line("Programming Language", 20, sf::Color(150, 200, 255));
    add_space(40);

    add_line("CMake", 28, sf::Color::Cyan);
    add_line("Build System", 20, sf::Color(150, 200, 255));
    add_space(40);

    add_line("Visual Studio 2022", 28, sf::Color::Cyan);
    add_line("Development Environment", 20, sf::Color(150, 200, 255));
    add_space(100);

    // Special Thanks Section
    add_line("SPECIAL THANKS", 36, sf::Color::Yellow);
    add_space(30);
    add_line("To everyone who supported this project", 24, sf::Color::White);
    add_space(50);
    add_line("Created for SET09107", 24, sf::Color(200, 200, 200));
    add_line("Advanced Database Systems", 20, sf::Color(150, 150, 150));
    add_space(100);

    // Legal Section
    add_line("COPYRIGHT & LICENSE", 36, sf::Color::Yellow);
    add_space(30);
    add_line("© 2024 Scott", 20, sf::Color(150, 150, 150));
    add_line("All Rights Reserved", 20, sf::Color(150, 150, 150));
    add_space(100);

    // Final Message
    add_line("THANK YOU FOR PLAYING!", 48, sf::Color::White);
    add_space(200);
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
            // Show main menu again
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

    // Auto-scroll credits
    if (_auto_scroll)
    {
        _scroll_position -= _scroll_speed * dt;

        // Reset scroll if we've gone past all credits
        if (!_credits_lines.empty())
        {
            float lastLineY = _credits_lines.back().getPosition().y;
            if (_scroll_position + lastLineY < -200.0f)
            {
                // Loop back to start
                _scroll_position = param::game_height;
            }
        }
    }

    // Manual scroll controls
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

    // Resume auto-scroll with Space
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

    // Update all credits line positions
    for (auto& line : _credits_lines)
    {
        sf::Vector2f pos = line.getPosition();
        pos.y = line.getPosition().y - _credits_lines[0].getPosition().y + _scroll_position;
        line.setPosition(pos.x, pos.y);
    }
}

void CreditsScene::render()
{
    // Render overlay
    Renderer::queue(&_overlay);

    // Render credits text (only if on screen)
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
        // Escape to go back
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

    // Handle button click
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
    _back_button.reset();
}
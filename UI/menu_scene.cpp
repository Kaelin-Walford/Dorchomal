#include "menu_scene.hpp"
#include "../game_parameters.hpp"
#include "renderer.hpp"
#include "../scenes.hpp"
#include <iostream>

using param = Parameters;

void MenuScene::load()
{
    // Load font
    if (!_font.loadFromFile("resources/fonts/pixelated.otf"))
    {
        std::cerr << "Failed to load font. Using default font." << std::endl;
    }

    // Setup overlay
    _overlay.setSize(sf::Vector2f(param::game_width, param::game_height));
    _overlay.setFillColor(sf::Color(0, 0, 0, 180));

    // Setup title text
    _title_text.setFont(_font);
    _title_text.setCharacterSize(72);
    _title_text.setFillColor(sf::Color::White);
    _title_text.setString("DORCHOMAL");

    // Center title text
    sf::FloatRect titleBounds = _title_text.getLocalBounds();
    _title_text.setOrigin(titleBounds.left + titleBounds.width / 2.f,
        titleBounds.top + titleBounds.height / 2.f);
    _title_text.setPosition(param::game_width / 2.f, 150.f);

    // Create menu buttons
    create_main_menu_buttons();
    create_pause_menu_buttons();

    // Start with main menu
    _menu_state = MenuState::MAIN_MENU;
}

void MenuScene::create_main_menu_buttons()
{
    float centerX = param::game_width / 2.f;
    float startY = param::game_height / 2.f - 120.f;  // Adjusted for more buttons
    float buttonWidth = 300.f;
    float buttonHeight = 60.f;
    float spacing = 20.f;

    int buttonIndex = 0;

    // Start Game button
    auto startBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * buttonIndex++),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Start Game",
        &_font
    );
    startBtn->set_callback([this]() {
        std::cout << "Starting game..." << std::endl;
        _menu_state = MenuState::PLAYING;
        GameSystem::set_active_scene(Scenes::kaelinsPlayground);
        });
    _main_menu_buttons.push_back(std::move(startBtn));

    // Load Game button (placeholder)
    auto loadBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * buttonIndex++),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Load Game",
        &_font
    );
    loadBtn->set_callback([this]() {
        std::cout << "Load game not implemented yet" << std::endl;
        });
    _main_menu_buttons.push_back(std::move(loadBtn));

    // Settings button (placeholder)
    auto settingsBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * buttonIndex++),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Settings",
        &_font
    );
    settingsBtn->set_callback([this]() {
        std::cout << "Settings not implemented yet" << std::endl;
        });
    _main_menu_buttons.push_back(std::move(settingsBtn));

    // Credits button (placeholder)
    auto creditsBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * buttonIndex++),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Credits",
        &_font
    );
    creditsBtn->set_callback([this]() {
        std::cout << "Credits not implemented yet" << std::endl;
        });
    _main_menu_buttons.push_back(std::move(creditsBtn));

    // Quit button
    auto quitBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * buttonIndex++),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Quit",
        &_font
    );
    quitBtn->set_callback([this]() {
        std::cout << "Quitting game..." << std::endl;
        exit(0);
        });
    _main_menu_buttons.push_back(std::move(quitBtn));
}

void MenuScene::create_pause_menu_buttons()
{
    float centerX = param::game_width / 2.f;
    float startY = param::game_height / 2.f - 90.f;
    float buttonWidth = 300.f;
    float buttonHeight = 60.f;
    float spacing = 20.f;

    // Resume button
    auto resumeBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Resume",
        &_font
    );
    resumeBtn->set_callback([this]() {
        std::cout << "Resuming game..." << std::endl;
        _menu_state = MenuState::PLAYING;
        hide_menus();
        if (Scenes::kaelinsPlayground) {
            Scenes::kaelinsPlayground->set_paused(false);
        }
        });
    _pause_menu_buttons.push_back(std::move(resumeBtn));

    // Restart button
    auto restartBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + buttonHeight + spacing),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Restart",
        &_font
    );
    restartBtn->set_callback([this]() {
        std::cout << "Restarting level..." << std::endl;
        Scenes::kaelinsPlayground->unload();
        Scenes::kaelinsPlayground->load();
        _menu_state = MenuState::PLAYING;
        if (Scenes::kaelinsPlayground) {
            Scenes::kaelinsPlayground->set_paused(false);
        }
        });
    _pause_menu_buttons.push_back(std::move(restartBtn));

    // Main Menu button
    auto mainMenuBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * 2),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Main Menu",
        &_font
    );
    mainMenuBtn->set_callback([this]() {
        std::cout << "Returning to main menu..." << std::endl;
        _menu_state = MenuState::MAIN_MENU;
        GameSystem::set_active_scene(Scenes::menuScene);
        });
    _pause_menu_buttons.push_back(std::move(mainMenuBtn));

    // Quit button
    auto quitBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, startY + (buttonHeight + spacing) * 3),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Quit",
        &_font
    );
    quitBtn->set_callback([this]() {
        std::cout << "Quitting game..." << std::endl;
        exit(0);
        });
    _pause_menu_buttons.push_back(std::move(quitBtn));
}

void MenuScene::update(const float& dt)
{
    Scene::update(dt);

    if (_menu_state == MenuState::MAIN_MENU)
    {
        _title_text.setString("DORCHOMAL");
    }
    else if (_menu_state == MenuState::PAUSED)
    {
        _title_text.setString("PAUSED");
    }

    // Center title text
    sf::FloatRect titleBounds = _title_text.getLocalBounds();
    _title_text.setOrigin(titleBounds.left + titleBounds.width / 2.f,
        titleBounds.top + titleBounds.height / 2.f);
    _title_text.setPosition(param::game_width / 2.f, 150.f);
}

void MenuScene::render()
{
    if (_menu_state != MenuState::PLAYING)
    {
        Renderer::queue(&_overlay);
        Renderer::queue(&_title_text);
    }
}

void MenuScene::show_main_menu()
{
    _menu_state = MenuState::MAIN_MENU;
}

void MenuScene::show_pause_menu()
{
    _menu_state = MenuState::PAUSED;
}

void MenuScene::hide_menus()
{
    _menu_state = MenuState::PLAYING;
}

void MenuScene::unload()
{
    _main_menu_buttons.clear();
    _pause_menu_buttons.clear();
}

void MenuScene::render_buttons(sf::RenderWindow& window)
{
    if (_menu_state == MenuState::MAIN_MENU)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (const auto& button : _main_menu_buttons)
        {
            button->update(mousePos);
            button->render(window);
        }
    }
    else if (_menu_state == MenuState::PAUSED)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (const auto& button : _pause_menu_buttons)
        {
            button->update(mousePos);
            button->render(window);
        }
    }
}

void MenuScene::handle_event(const sf::Event& event, sf::RenderWindow& window)
{
    if (_menu_state == MenuState::PLAYING) return;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    if (_menu_state == MenuState::MAIN_MENU)
    {
        for (const auto& button : _main_menu_buttons)
        {
            button->is_clicked(event, mousePos);
        }
    }
    else if (_menu_state == MenuState::PAUSED)
    {
        for (const auto& button : _pause_menu_buttons)
        {
            button->is_clicked(event, mousePos);
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            if (Scenes::kaelinsPlayground) {
                Scenes::kaelinsPlayground->toggle_pause();
            }
        }
    }
}
#include "settings_scene.hpp"
#include "menu_scene.hpp"
#include "../game_parameters.hpp"
#include "../engine/renderer.hpp"
#include "../scenes.hpp"
#include <iostream>

using param = Parameters;

void SettingsScene::load()
{
    // Load font
    if (!_font.loadFromFile("resources/fonts/pixelated.otf"))
    {
        std::cerr << "Failed to load font for settings" << std::endl;
    }

    if (!_number_font.loadFromFile("resources/fonts/CreditCard-26Me.ttf"))
    {
        std::cerr << "Failed to load number font for settings" << std::endl;
    }

    // Setup overlay
    _overlay.setSize(sf::Vector2f(param::game_width, param::game_height));
    _overlay.setFillColor(sf::Color(0, 0, 0, 200));

    // Setup title
    _title_text.setFont(_font);
    _title_text.setCharacterSize(60);
    _title_text.setFillColor(sf::Color::White);
    _title_text.setString("SETTINGS");
    sf::FloatRect titleBounds = _title_text.getLocalBounds();
    _title_text.setOrigin(titleBounds.left + titleBounds.width / 2.f,
        titleBounds.top + titleBounds.height / 2.f);
    _title_text.setPosition(param::game_width / 2.f, 80.f);

    // Initialize state
    _current_tab = SettingsTab::AUDIO;
    _return_scene = nullptr;
    _waiting_for_key = false;
    _rebinding_index = -1;

    // Create UI elements
    create_tab_buttons();
    create_audio_controls();
    create_display_controls();
    create_control_controls();
    create_bottom_buttons();
}


void SettingsScene::create_tab_buttons()
{
    float centerX = param::game_width / 2.f;
    float startX = centerX - 350.f;
    float y = 170.f;
    float buttonWidth = 220.f;
    float buttonHeight = 50.f;
    float spacing = 20.f;

    // Audio tab
    auto audioTab = std::make_unique<UIButton>(
        sf::Vector2f(startX, y),
        sf::Vector2f(buttonWidth, buttonHeight),
        "AUDIO",
        &_font
    );
    audioTab->set_callback([this]() { switch_to_tab(SettingsTab::AUDIO); });
    _tab_buttons.push_back(std::move(audioTab));

    // Display tab
    auto displayTab = std::make_unique<UIButton>(
        sf::Vector2f(startX + buttonWidth + spacing, y),
        sf::Vector2f(buttonWidth, buttonHeight),
        "DISPLAY",
        &_font
    );
    displayTab->set_callback([this]() { switch_to_tab(SettingsTab::DISPLAY); });
    _tab_buttons.push_back(std::move(displayTab));

    // Controls tab
    auto controlsTab = std::make_unique<UIButton>(
        sf::Vector2f(startX + (buttonWidth + spacing) * 2, y),
        sf::Vector2f(buttonWidth, buttonHeight),
        "CONTROLS",
        &_font
    );
    controlsTab->set_callback([this]() { switch_to_tab(SettingsTab::CONTROLS); });
    _tab_buttons.push_back(std::move(controlsTab));
}

void SettingsScene::create_audio_controls()
{
    auto& settings = GameSettings::get_instance();
    float startX = 400.f;
    float startY = 280.f;
    float spacing = 90.f;

    // Master Volume
    auto masterVol = std::make_unique<UISlider>(
        sf::Vector2f(startX, startY),
        500.f, 0.f, 100.f,
        settings.master_volume,
        "Master Volume",
        &_font, &_number_font
    );
    masterVol->set_callback([](float val) {
        GameSettings::get_instance().master_volume = val;
        GameSettings::get_instance().apply_audio_settings();
        });
    _audio_sliders.push_back(std::move(masterVol));

    // Music Volume
    auto musicVol = std::make_unique<UISlider>(
        sf::Vector2f(startX, startY + spacing),
        500.f, 0.f, 100.f,
        settings.music_volume,
        "Music Volume",
        &_font, &_number_font
    );
    musicVol->set_callback([](float val) {
        GameSettings::get_instance().music_volume = val;
        GameSettings::get_instance().apply_audio_settings();
        });
    _audio_sliders.push_back(std::move(musicVol));

    // SFX Volume
    auto sfxVol = std::make_unique<UISlider>(
        sf::Vector2f(startX, startY + spacing * 2),
        500.f, 0.f, 100.f,
        settings.sfx_volume,
        "Sound Effects Volume",
        &_font, &_number_font
    );
    sfxVol->set_callback([](float val) {
        GameSettings::get_instance().sfx_volume = val;
        GameSettings::get_instance().apply_audio_settings();
        });
    _audio_sliders.push_back(std::move(sfxVol));

    // Audio Enable Toggle
    auto audioToggle = std::make_unique<UIToggle>(
        sf::Vector2f(startX, startY + spacing * 3),
        "Enable Audio",
        settings.audio_enabled,
        &_font
    );
    audioToggle->set_callback([](bool val) {
        GameSettings::get_instance().audio_enabled = val;
        GameSettings::get_instance().apply_audio_settings();
        });
    _audio_toggles.push_back(std::move(audioToggle));
}

void SettingsScene::create_display_controls()
{
    auto& settings = GameSettings::get_instance();
    float startX = 400.f;
    float startY = 280.f;
    float spacing = 90.f;

    // Fullscreen Toggle
    auto fullscreenToggle = std::make_unique<UIToggle>(
        sf::Vector2f(startX, startY),
        "Fullscreen",
        settings.fullscreen,
        &_font
    );
    fullscreenToggle->set_callback([](bool val) {
        GameSettings::get_instance().fullscreen = val;
        });
    _display_toggles.push_back(std::move(fullscreenToggle));

    // VSync Toggle
    auto vsyncToggle = std::make_unique<UIToggle>(
        sf::Vector2f(startX, startY + spacing),
        "VSync",
        settings.vsync,
        &_font
    );
    vsyncToggle->set_callback([](bool val) {
        GameSettings::get_instance().vsync = val;
        });
    _display_toggles.push_back(std::move(vsyncToggle));


    // Resolution text
    // Resolution label
    _resolution_label.setFont(_font);
    _resolution_label.setCharacterSize(20);
    _resolution_label.setFillColor(sf::Color::White);
    _resolution_label.setPosition(startX, startY + spacing * 3);
    _resolution_label.setString("Resolution:");

    // Resolution value
    _resolution_value.setFont(_number_font);
    _resolution_value.setCharacterSize(20);
    _resolution_value.setFillColor(sf::Color::White);
    _resolution_value.setPosition(startX + 120, startY + spacing * 3);
    _resolution_value.setString(std::to_string(settings.resolution_width) + "x" +
        std::to_string(settings.resolution_height));


    // Resolution buttons
    auto resolutionBtn = std::make_unique<UIButton>(
        sf::Vector2f(startX + 300.f, startY + spacing * 3),
        sf::Vector2f(200.f, 40.f),
        "Change",
        &_font
    );
    resolutionBtn->set_callback([this]() {
        auto& settings = GameSettings::get_instance();
        settings.current_resolution_index =
            (settings.current_resolution_index + 1) % settings.available_resolutions.size();
        auto& res = settings.available_resolutions[settings.current_resolution_index];
        settings.resolution_width = res.width;
        settings.resolution_height = res.height;
        _resolution_value.setString(
            std::to_string(settings.resolution_width) + "x" +
            std::to_string(settings.resolution_height));
        });
    _display_buttons.push_back(std::move(resolutionBtn));

}

void SettingsScene::create_control_controls()
{
    auto& settings = GameSettings::get_instance();
    float startX = 350.f;
    float startY = 280.f;
    float spacing = 45.f;

    // Control labels and keybinds
    std::string labels[] = {
        "Move Left:", "Move Right:", "Jump:", "Look Up:",
        "Look Down:", "Dash:", "Fireball:", "Melee Attack:", "Pause:"
    };

    for (int i = 0; i < 9; i++)
    {
        _keybind_labels[i].setFont(_font);
        _keybind_labels[i].setCharacterSize(20);
        _keybind_labels[i].setFillColor(sf::Color::White);
        _keybind_labels[i].setPosition(startX, startY + i * spacing);
        _keybind_labels[i].setString(labels[i]);

        _keybind_values[i].setFont(_font);
        _keybind_values[i].setCharacterSize(20);
        _keybind_values[i].setFillColor(sf::Color(150, 200, 255));
        _keybind_values[i].setPosition(startX + 250.f, startY + i * spacing);
    }

    update_control_display();

    // Mouse Sensitivity
    auto sensitivitySlider = std::make_unique<UISlider>(
        sf::Vector2f(startX + 600.f, startY),
        400.f, 0.1f, 2.0f,
        settings.mouse_sensitivity,
        "Mouse Sensitivity",
        &_font
    );
    sensitivitySlider->set_callback([](float val) {
        GameSettings::get_instance().mouse_sensitivity = val;
        });
    _control_sliders.push_back(std::move(sensitivitySlider));

    // Invert Mouse Y
    auto invertToggle = std::make_unique<UIToggle>(
        sf::Vector2f(startX + 600.f, startY + 100.f),
        "Invert Mouse Y",
        settings.invert_mouse_y,
        &_font
    );
    invertToggle->set_callback([](bool val) {
        GameSettings::get_instance().invert_mouse_y = val;
        });
    _control_toggles.push_back(std::move(invertToggle));

    // Rebind buttons
    for (int i = 0; i < 9; i++)
    {
        auto rebindBtn = std::make_unique<UIButton>(
            sf::Vector2f(startX + 400.f, startY + i * spacing - 10.f),
            sf::Vector2f(100.f, 35.f),
            "Rebind",
            &_font
        );
        rebindBtn->set_callback([this, i]() {
            _waiting_for_key = true;
            _rebinding_index = i;
            _keybind_values[i].setString("Press key...");
            _keybind_values[i].setFillColor(sf::Color::Yellow);
            });
        _control_buttons.push_back(std::move(rebindBtn));
    }
}

void SettingsScene::create_bottom_buttons()
{
    float centerX = param::game_width / 2.f;
    float y = param::game_height - 100.f;
    float buttonWidth = 180.f;
    float buttonHeight = 50.f;
    float spacing = 30.f;

    // Apply button
    auto applyBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX - buttonWidth - spacing, y),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Apply",
        &_font
    );
    applyBtn->set_callback([this]() { apply_settings(); });
    _bottom_buttons.push_back(std::move(applyBtn));

    // Reset button
    auto resetBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX, y),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Reset Defaults",
        &_font
    );
    resetBtn->set_callback([this]() { reset_settings(); });
    _bottom_buttons.push_back(std::move(resetBtn));

    // Back button
    auto backBtn = std::make_unique<UIButton>(
        sf::Vector2f(centerX + buttonWidth + spacing, y),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Back",
        &_font
    );
    backBtn->set_callback([this]() {
        if (_return_scene) {
            // Shows menu buttons again 
            if (Scenes::menuScene) {
                Scenes::menuScene->show_main_menu();
            }
            GameSystem::set_active_scene(_return_scene);
        }
        });
    _bottom_buttons.push_back(std::move(backBtn));
}

void SettingsScene::switch_to_tab(SettingsTab tab)
{
    _current_tab = tab;
    std::cout << "Switched to tab: " << static_cast<int>(tab) << std::endl;
}

void SettingsScene::apply_settings()
{
    auto& settings = GameSettings::get_instance();

    // Apply audio immediately
    settings.apply_audio_settings();

    // Apply display settings immediately
    settings.apply_display_settings();

    // Save to file
    settings.save_to_file();

    std::cout << "Settings applied and saved!" << std::endl;
}

void SettingsScene::reset_settings()
{
    GameSettings::get_instance().reset_to_defaults();

    // Reload controls with default values
    unload();
    load();

    std::cout << "Settings reset to defaults!" << std::endl;
}

void SettingsScene::update_control_display()
{
    auto& settings = GameSettings::get_instance();

    _keybind_values[0].setString(GameSettings::get_key_name(settings.key_move_left));
    _keybind_values[1].setString(GameSettings::get_key_name(settings.key_move_right));
    _keybind_values[2].setString(GameSettings::get_key_name(settings.key_jump));
    _keybind_values[3].setString(GameSettings::get_key_name(settings.key_look_up));
    _keybind_values[4].setString(GameSettings::get_key_name(settings.key_look_down));
    _keybind_values[5].setString(GameSettings::get_key_name(settings.key_dash));
    _keybind_values[6].setString(GameSettings::get_key_name(settings.key_fireball));
    _keybind_values[7].setString(GameSettings::get_key_name(settings.key_melee));
    _keybind_values[8].setString(GameSettings::get_key_name(settings.key_pause));

    for (int i = 0; i < 9; i++)
    {
        _keybind_values[i].setFillColor(sf::Color(150, 200, 255));
    }
}

void SettingsScene::update(const float& dt)
{
    Scene::update(dt);
}

void SettingsScene::render()
{
    Renderer::queue(&_overlay);
    Renderer::queue(&_title_text);

    // Render tab content
    if (_current_tab == SettingsTab::AUDIO)
    {
        // Audio content 
    }
    else if (_current_tab == SettingsTab::DISPLAY)
    {
        Renderer::queue(&_resolution_label); Renderer::queue(&_resolution_value);
    }
    else if (_current_tab == SettingsTab::CONTROLS)
    {
        for (int i = 0; i < 9; i++)
        {
            Renderer::queue(&_keybind_labels[i]);
            Renderer::queue(&_keybind_values[i]);
        }
    }
}

void SettingsScene::render_ui(sf::RenderWindow& window)
{
    // Render tab buttons
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (const auto& btn : _tab_buttons)
    {
        btn->update(mousePos);
        btn->render(window);
    }

    // Render tab content
    bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (_current_tab == SettingsTab::AUDIO)
    {
        for (const auto& slider : _audio_sliders)
        {
            slider->update(mousePos, mouse_pressed);
            slider->render(window);
        }
        for (const auto& toggle : _audio_toggles)
        {
            toggle->update(mousePos);
            toggle->render(window);
        }
    }
    else if (_current_tab == SettingsTab::DISPLAY)
    {
        for (const auto& toggle : _display_toggles)
        {
            toggle->update(mousePos);
            toggle->render(window);
        }
        for (const auto& btn : _display_buttons)
        {
            btn->update(mousePos);
            btn->render(window);
        }
        window.draw(_resolution_label);
        window.draw(_resolution_value);
    }
    else if (_current_tab == SettingsTab::CONTROLS)
    {
        for (const auto& slider : _control_sliders)
        {
            slider->update(mousePos, mouse_pressed);
            slider->render(window);
        }
        for (const auto& toggle : _control_toggles)
        {
            toggle->update(mousePos);
            toggle->render(window);
        }
        for (const auto& btn : _control_buttons)
        {
            btn->update(mousePos);
            btn->render(window);
        }
        for (int i = 0; i < 9; i++)
        {
            window.draw(_keybind_labels[i]);
            window.draw(_keybind_values[i]);
        }
    }

    // Render bottom buttons
    for (const auto& btn : _bottom_buttons)
    {
        btn->update(mousePos);
        btn->render(window);
    }
}

void SettingsScene::handle_event(const sf::Event& event, sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // Handle key rebinding
    if (_waiting_for_key && event.type == sf::Event::KeyPressed)
    {
        auto& settings = GameSettings::get_instance();
        sf::Keyboard::Key newKey = event.key.code;

        // Assign new key based on which control is being rebound
        switch (_rebinding_index)
        {
        case 0: settings.key_move_left = newKey; break;
        case 1: settings.key_move_right = newKey; break;
        case 2: settings.key_jump = newKey; break;
        case 3: settings.key_look_up = newKey; break;
        case 4: settings.key_look_down = newKey; break;
        case 5: settings.key_dash = newKey; break;
        case 6: settings.key_fireball = newKey; break;
        case 7: settings.key_melee = newKey; break;
        case 8: settings.key_pause = newKey; break;
        }

        _waiting_for_key = false;
        _rebinding_index = -1;
        update_control_display();
        return;
    }

    // Handle tab clicks
    for (const auto& btn : _tab_buttons)
    {
        btn->is_clicked(event, mousePos);
    }

    // Handle clicks
    if (_current_tab == SettingsTab::AUDIO)
    {
        for (const auto& toggle : _audio_toggles)
        {
            toggle->is_clicked(event, mousePos);
        }
    }
    else if (_current_tab == SettingsTab::DISPLAY)
    {
        for (const auto& toggle : _display_toggles)
        {
            toggle->is_clicked(event, mousePos);
        }
        for (const auto& btn : _display_buttons)
        {
            btn->is_clicked(event, mousePos);
        }
    }
    else if (_current_tab == SettingsTab::CONTROLS)
    {
        for (const auto& toggle : _control_toggles)
        {
            toggle->is_clicked(event, mousePos);
        }
        for (const auto& btn : _control_buttons)
        {
            btn->is_clicked(event, mousePos);
        }
    }

    // Handle button clicks
    for (const auto& btn : _bottom_buttons)
    {
        btn->is_clicked(event, mousePos);
    }
}

void SettingsScene::unload()
{
    _tab_buttons.clear();
    _audio_sliders.clear();
    _audio_toggles.clear();
    _display_toggles.clear();
    _display_buttons.clear();
    _control_buttons.clear();
    _control_sliders.clear();
    _control_toggles.clear();
    _bottom_buttons.clear();
}
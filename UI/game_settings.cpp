#include "game_settings.hpp"
#include <fstream>
#include <iostream>

std::string GameSettings::get_key_name(sf::Keyboard::Key key)
{
    // Map common keys to readable names
    static const std::map<sf::Keyboard::Key, std::string> key_names = {
        {sf::Keyboard::A, "A"}, {sf::Keyboard::B, "B"}, {sf::Keyboard::C, "C"},
        {sf::Keyboard::D, "D"}, {sf::Keyboard::E, "E"}, {sf::Keyboard::F, "F"},
        {sf::Keyboard::G, "G"}, {sf::Keyboard::H, "H"}, {sf::Keyboard::I, "I"},
        {sf::Keyboard::J, "J"}, {sf::Keyboard::K, "K"}, {sf::Keyboard::L, "L"},
        {sf::Keyboard::M, "M"}, {sf::Keyboard::N, "N"}, {sf::Keyboard::O, "O"},
        {sf::Keyboard::P, "P"}, {sf::Keyboard::Q, "Q"}, {sf::Keyboard::R, "R"},
        {sf::Keyboard::S, "S"}, {sf::Keyboard::T, "T"}, {sf::Keyboard::U, "U"},
        {sf::Keyboard::V, "V"}, {sf::Keyboard::W, "W"}, {sf::Keyboard::X, "X"},
        {sf::Keyboard::Y, "Y"}, {sf::Keyboard::Z, "Z"},
        {sf::Keyboard::Num0, "0"}, {sf::Keyboard::Num1, "1"},
        {sf::Keyboard::Num2, "2"}, {sf::Keyboard::Num3, "3"},
        {sf::Keyboard::Num4, "4"}, {sf::Keyboard::Num5, "5"},
        {sf::Keyboard::Num6, "6"}, {sf::Keyboard::Num7, "7"},
        {sf::Keyboard::Num8, "8"}, {sf::Keyboard::Num9, "9"},
        {sf::Keyboard::Space, "Space"}, {sf::Keyboard::Enter, "Enter"},
        {sf::Keyboard::Escape, "Escape"}, {sf::Keyboard::LShift, "Left Shift"},
        {sf::Keyboard::RShift, "Right Shift"}, {sf::Keyboard::LControl, "Left Ctrl"},
        {sf::Keyboard::RControl, "Right Ctrl"}, {sf::Keyboard::LAlt, "Left Alt"},
        {sf::Keyboard::RAlt, "Right Alt"}, {sf::Keyboard::Tab, "Tab"},
        {sf::Keyboard::Left, "Left Arrow"}, {sf::Keyboard::Right, "Right Arrow"},
        {sf::Keyboard::Up, "Up Arrow"}, {sf::Keyboard::Down, "Down Arrow"}
    };

    auto it = key_names.find(key);
    if (it != key_names.end())
        return it->second;

    return "Unknown";
}

std::string GameSettings::get_mouse_button_name(sf::Mouse::Button button)
{
    switch (button)
    {
    case sf::Mouse::Left: return "Left Click";
    case sf::Mouse::Right: return "Right Click";
    case sf::Mouse::Middle: return "Middle Click";
    case sf::Mouse::XButton1: return "Mouse 4";
    case sf::Mouse::XButton2: return "Mouse 5";
    default: return "Unknown";
    }
}

void GameSettings::apply_display_settings(sf::RenderWindow& window)
{
    // resolution and fullscreen
    sf::VideoMode mode(resolution_width, resolution_height);

    if (fullscreen)
    {
        window.create(mode, "Dorchomal", sf::Style::Fullscreen);
    }
    else
    {
        window.create(mode, "Dorchomal", sf::Style::Close | sf::Style::Titlebar);
    }

    // Apply VSync
    window.setVerticalSyncEnabled(vsync);

    // Apply FPS limit
    if (fps_limit > 0)
    {
        window.setFramerateLimit(fps_limit);
    }
    else
    {
        window.setFramerateLimit(0); // Unlimited
    }
}

void GameSettings::apply_audio_settings()
{
    // audio system when we have one
    std::cout << "Audio settings applied:" << std::endl;
    std::cout << "  Master: " << master_volume << "%" << std::endl;
    std::cout << "  Music: " << music_volume << "%" << std::endl;
    std::cout << "  SFX: " << sfx_volume << "%" << std::endl;
    std::cout << "  Enabled: " << (audio_enabled ? "Yes" : "No") << std::endl;
}

bool GameSettings::save_to_file(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to save settings to " << filename << std::endl;
        return false;
    }

    // Audio settings
    file << "[Audio]" << std::endl;
    file << "master_volume=" << master_volume << std::endl;
    file << "music_volume=" << music_volume << std::endl;
    file << "sfx_volume=" << sfx_volume << std::endl;
    file << "audio_enabled=" << audio_enabled << std::endl;
    file << std::endl;

    // Display settings
    file << "[Display]" << std::endl;
    file << "fullscreen=" << fullscreen << std::endl;
    file << "resolution_width=" << resolution_width << std::endl;
    file << "resolution_height=" << resolution_height << std::endl;
    file << "vsync=" << vsync << std::endl;
    file << "show_fps=" << show_fps << std::endl;
    file << "fps_limit=" << fps_limit << std::endl;
    file << std::endl;

    // Control settings
    file << "[Controls]" << std::endl;
    file << "key_move_left=" << static_cast<int>(key_move_left) << std::endl;
    file << "key_move_right=" << static_cast<int>(key_move_right) << std::endl;
    file << "key_jump=" << static_cast<int>(key_jump) << std::endl;
    file << "key_look_up=" << static_cast<int>(key_look_up) << std::endl;
    file << "key_look_down=" << static_cast<int>(key_look_down) << std::endl;
    file << "key_dash=" << static_cast<int>(key_dash) << std::endl;
    file << "key_fireball=" << static_cast<int>(key_fireball) << std::endl;
    file << "key_melee=" << static_cast<int>(key_melee) << std::endl;
    file << "key_pause=" << static_cast<int>(key_pause) << std::endl;
    file << "mouse_fireball=" << static_cast<int>(mouse_fireball) << std::endl;
    file << "mouse_sensitivity=" << mouse_sensitivity << std::endl;
    file << "invert_mouse_y=" << invert_mouse_y << std::endl;

    file.close();
    std::cout << "Settings saved to " << filename << std::endl;
    return true;
}

bool GameSettings::load_from_file(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Settings file not found: " << filename << std::endl;
        return false;
    }

    std::string line;
    std::string current_section;

    while (std::getline(file, line))
    {
        // Skip empty lines
        if (line.empty()) continue;

        // Section headers
        if (line[0] == '[')
        {
            current_section = line;
            continue;
        }

        // Parse key=value pairs
        size_t equals_pos = line.find('=');
        if (equals_pos != std::string::npos)
        {
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);

            // Audio settings
            if (key == "master_volume") master_volume = std::stof(value);
            else if (key == "music_volume") music_volume = std::stof(value);
            else if (key == "sfx_volume") sfx_volume = std::stof(value);
            else if (key == "audio_enabled") audio_enabled = (value == "1");

            // Display settings
            else if (key == "fullscreen") fullscreen = (value == "1");
            else if (key == "resolution_width") resolution_width = std::stoi(value);
            else if (key == "resolution_height") resolution_height = std::stoi(value);
            else if (key == "vsync") vsync = (value == "1");
            else if (key == "show_fps") show_fps = (value == "1");
            else if (key == "fps_limit") fps_limit = std::stoi(value);

            // Control settings
            else if (key == "key_move_left") key_move_left = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_move_right") key_move_right = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_jump") key_jump = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_look_up") key_look_up = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_look_down") key_look_down = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_dash") key_dash = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_fireball") key_fireball = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_melee") key_melee = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "key_pause") key_pause = static_cast<sf::Keyboard::Key>(std::stoi(value));
            else if (key == "mouse_fireball") mouse_fireball = static_cast<sf::Mouse::Button>(std::stoi(value));
            else if (key == "mouse_sensitivity") mouse_sensitivity = std::stof(value);
            else if (key == "invert_mouse_y") invert_mouse_y = (value == "1");
        }
    }

    file.close();
    std::cout << "Settings loaded from " << filename << std::endl;
    return true;
}

void GameSettings::reset_to_defaults()
{
    // Audio defaults
    master_volume = 100.f;
    music_volume = 80.f;
    sfx_volume = 80.f;
    audio_enabled = true;

    // Display defaults
    fullscreen = false;
    resolution_width = 1920;
    resolution_height = 1080;
    vsync = true;
    show_fps = false;
    fps_limit = 60;
    current_resolution_index = 0;

    // Control defaults
    key_move_left = sf::Keyboard::A;
    key_move_right = sf::Keyboard::D;
    key_jump = sf::Keyboard::Space;
    key_look_up = sf::Keyboard::W;
    key_look_down = sf::Keyboard::S;
    key_dash = sf::Keyboard::LShift;
    key_fireball = sf::Keyboard::LControl;
    key_melee = sf::Keyboard::E;
    key_pause = sf::Keyboard::Escape;
    mouse_fireball = sf::Mouse::Left;
    mouse_sensitivity = 1.0f;
    invert_mouse_y = false;

    std::cout << "Settings reset to defaults" << std::endl;
}
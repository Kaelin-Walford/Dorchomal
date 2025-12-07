#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <map>
#include <vector>  

// Settings data structure 
class GameSettings
{
public:
    // Singleton pattern 
    static GameSettings& get_instance()
    {
        static GameSettings instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    GameSettings(const GameSettings&) = delete;
    GameSettings& operator=(const GameSettings&) = delete;

    // AUDIO SETTINGS

    float master_volume = 100.f;      // 0-100
    float music_volume = 80.f;        // 0-100
    float sfx_volume = 80.f;          // 0-100
    bool audio_enabled = true;        // Master mute toggle

    // DISPLAY SETTINGS

    bool fullscreen = false;
    int resolution_width = 1920;
    int resolution_height = 1080;
    bool vsync = true;

    // Available resolutions
    struct Resolution {
        int width;
        int height;
        std::string name;
    };

    std::vector<Resolution> available_resolutions = {
        {1920, 1080, "1920x1080 (Full HD)"},
        {1600, 900, "1600x900"},
        {1366, 768, "1366x768"},
        {1280, 720, "1280x720 (HD)"},
        {1024, 768, "1024x768"},
        {800, 600, "800x600"}
    };

    int current_resolution_index = 0;

    // CONTROL SETTINGS

    // Key bindings (default values from game_parameters.hpp)
    sf::Keyboard::Key key_move_left = sf::Keyboard::A;
    sf::Keyboard::Key key_move_right = sf::Keyboard::D;
    sf::Keyboard::Key key_jump = sf::Keyboard::Space;
    sf::Keyboard::Key key_look_up = sf::Keyboard::W;
    sf::Keyboard::Key key_look_down = sf::Keyboard::S;
    sf::Keyboard::Key key_dash = sf::Keyboard::LShift;
    sf::Keyboard::Key key_fireball = sf::Keyboard::LControl;
    sf::Keyboard::Key key_melee = sf::Keyboard::E;
    sf::Keyboard::Key key_pause = sf::Keyboard::Escape;

    sf::Mouse::Button mouse_fireball = sf::Mouse::Left;

    // Mouse settings
    float mouse_sensitivity = 1.0f;   // 0.1 - 2.0
    bool invert_mouse_y = false;

    // METHODS

    // Get key name as string
    static std::string get_key_name(sf::Keyboard::Key key);

    // Get mouse button name as string
    static std::string get_mouse_button_name(sf::Mouse::Button button);

    // Apply display settings to window
    void apply_display_settings();

    // Apply audio settings (would connect to audio system)
    void apply_audio_settings();

    // Save settings to file
    bool save_to_file(const std::string& filename = "settings.cfg");

    // Load settings from file
    bool load_from_file(const std::string& filename = "settings.cfg");

    // Reset to defaults
    void reset_to_defaults();

private:
    GameSettings() = default;  // Private constructor for singleton
};
#pragma once
#include "game_system.hpp"
#include "ui_button.hpp"
#include "ui_slider.hpp"
#include "ui_toggle.hpp"
#include "game_settings.hpp"
#include <memory>
#include <vector>

enum class SettingsTab
{
    AUDIO,
    DISPLAY,
    CONTROLS
};

class SettingsScene : public Scene
{
public:
    SettingsScene() = default;
    void load() override;
    void update(const float& dt) override;
    void render() override;
    void unload() override;

    // Helper methods for event handling
    void render_ui(sf::RenderWindow& window);
    void handle_event(const sf::Event& event, sf::RenderWindow& window);

    void set_return_scene(std::shared_ptr<Scene> scene) { _return_scene = scene; }

private:
    sf::Font _font;
    SettingsTab _current_tab;
    std::shared_ptr<Scene> _return_scene;  

    // Background
    sf::RectangleShape _overlay;
    sf::Text _title_text;

    // Tab buttons
    std::vector<std::unique_ptr<UIButton>> _tab_buttons;

    // Audio settings
    std::vector<std::unique_ptr<UISlider>> _audio_sliders;
    std::vector<std::unique_ptr<UIToggle>> _audio_toggles;

    // Display settings
    std::vector<std::unique_ptr<UIToggle>> _display_toggles;
    std::vector<std::unique_ptr<UIButton>> _display_buttons;
    sf::Text _resolution_text;
    sf::Text _fps_limit_text;

    // Controls settings
    std::vector<std::unique_ptr<UIButton>> _control_buttons;
    std::vector<std::unique_ptr<UISlider>> _control_sliders;
    std::vector<std::unique_ptr<UIToggle>> _control_toggles;
    sf::Text _keybind_labels[9];  // Labels for key bindings
    sf::Text _keybind_values[9];  // Current key assignments
    bool _waiting_for_key;
    int _rebinding_index;

    // Bottom buttons (Apply, Reset, Back)
    std::vector<std::unique_ptr<UIButton>> _bottom_buttons;

    // Creation methods
    void create_tab_buttons();
    void create_audio_controls();
    void create_display_controls();
    void create_control_controls();
    void create_bottom_buttons();

    // Helper methods
    void switch_to_tab(SettingsTab tab);
    void apply_settings();
    void reset_settings();
    void update_control_display();
};
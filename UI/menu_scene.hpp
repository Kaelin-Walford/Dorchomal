#pragma once
#include "game_system.hpp"
#include "ui_button.hpp"
#include <memory>
#include <vector>

enum class MenuState
{
    MAIN_MENU,
    PAUSED,
    PLAYING
};

class MenuScene : public Scene
{
public:
    MenuScene() = default;
    void load() override;
    void update(const float& dt) override;
    void render() override;
    void unload() override;

    void show_main_menu();
    void show_pause_menu(const std::string& level_name = "");
    void hide_menus();

    MenuState get_state() const { return _menu_state; }

    // Helper methods to be called from GameSystem's event/render loop
    void render_buttons(sf::RenderWindow& window);
    void handle_event(const sf::Event& event, sf::RenderWindow& window);

private:
    sf::Font _font;
    MenuState _menu_state;

    // Main menu buttons
    std::vector<std::unique_ptr<UIButton>> _main_menu_buttons;

    // Pause menu buttons
    std::vector<std::unique_ptr<UIButton>> _pause_menu_buttons;

    // Background overlay
    sf::RectangleShape _overlay;
    sf::Text _title_text;

    void create_main_menu_buttons();
    void create_pause_menu_buttons();
};
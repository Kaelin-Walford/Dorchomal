#pragma once
#include "../engine/game_system.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class UIButton;

class CreditsScene : public Scene
{
public:
    CreditsScene() = default;
    ~CreditsScene() override = default;

    void load() override;
    void unload() override;
    void update(const float& dt) override;
    void render() override;
    void handle_event(const sf::Event& event, sf::RenderWindow& window);

    void set_return_scene(std::shared_ptr<Scene> scene) { _return_scene = scene; }

private:
    sf::Font _font;
    sf::Font _number_font;
    sf::RectangleShape _overlay;

    // Credits text
    std::vector<sf::Text> _credits_lines;
    std::vector<float> _base_positions;
    float _scroll_position;
    float _scroll_speed;
    bool _auto_scroll;

    // Back button
    std::unique_ptr<UIButton> _back_button;

    // Return scene
    std::shared_ptr<Scene> _return_scene;

    void create_credits_text();
    void create_back_button();
};
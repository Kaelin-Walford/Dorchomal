#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

class LevelSystem {
public:
	enum Tile { EMPTY, START, END, WALL, ENEMY, WAYPOINT };

	static void load_level(const std::string& path, float tile_size = 30.f);
	static void unload_level();
	static void render(sf::RenderWindow& window);

	static Tile get_tile(sf::Vector2u);
	static Tile get_tile_at(const sf::Vector2f& v);
	static bool is_on_grid(sf::Vector2f v);
	static size_t get_width();
	static size_t get_height();
	static sf::Vector2f get_offset();
	static void set_offset(const sf::Vector2f& offset);
	static float get_tile_size();

	static sf::Vector2f get_tile_position(sf::Vector2u p);
	static sf::Vector2f get_tile_position(const sf::Vector2i& p);
	static std::vector<sf::Vector2u> find_tiles(Tile type);
	static std::vector<std::vector<sf::Vector2i>> get_groups(Tile type);
	static sf::Color get_color(Tile t);
	static void set_color(Tile t, sf::Color c);
	static bool in_group(const sf::Vector2i& tile, const std::vector<sf::Vector2i>& group);
	static Tile get_tile(const sf::Vector2i& p);
	static sf::Vector2f get_start_position();

protected:
	static std::unique_ptr<Tile[]> _tiles;
	static size_t _width;
	static size_t _height;
	static sf::Vector2f _offset;
	static float _tile_size;
	static std::map<Tile, sf::Color> _colors;
	static sf::Vector2f _start_position;

	// Texture for grass/wall tiles
	static std::shared_ptr<sf::Texture> _wall_texture;
	static bool _use_texture;

	LevelSystem() = delete;
	~LevelSystem() = delete;
};

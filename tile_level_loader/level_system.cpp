#include "level_system.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace sf;

std::unique_ptr<LevelSystem::Tile[]> LevelSystem::_tiles;
size_t LevelSystem::_width;
size_t LevelSystem::_height;
Vector2f LevelSystem::_offset(0.0f, 0.0f);
float LevelSystem::_tile_size = 30.0f;
map<LevelSystem::Tile, Color> LevelSystem::_colors{
    {WALL, Color::White},
    {END, Color::Red},
    {START, Color::Green},
    {EMPTY, Color::Transparent},
    {WAYPOINT, Color::Yellow},
    {ENEMY, Color::Magenta}
};
Vector2f LevelSystem::_start_position(0.0f, 0.0f);
std::shared_ptr<sf::Texture> LevelSystem::_wall_texture = nullptr;
bool LevelSystem::_use_texture = false;

void LevelSystem::load_level(const string& path, float tile_size) {
    _tile_size = tile_size;
    size_t w = 0, h = 0;
    string buffer;

    // Load level file
    ifstream f(path);
    if (!f.good()) {
        cerr << "Couldn't open level file: " << path << endl;
        return;
    }

    // Count dimensions
    while (getline(f, buffer)) {
        if (w == 0) {
            w = buffer.length();
        }
        h++;
    }
    if (h == 0 || w == 0) {
        cerr << "Level file empty or corrupted: " << path << endl;
        return;
    }

    _width = w;
    _height = h;
    _tiles = make_unique<Tile[]>(w * h);

    // Reset file to beginning
    f.clear();
    f.seekg(0, ios::beg);

    // Parse level data
    size_t row = 0;
    while (getline(f, buffer)) {
        for (size_t col = 0; col < buffer.length(); col++) {
            const char c = buffer[col];
            Tile tile = EMPTY;

            switch (c) {
            case 'w':
                tile = WALL;
                break;
            case 's':
                tile = START;
                _start_position = get_tile_position(Vector2u(static_cast<unsigned int>(col), 
                                                     static_cast<unsigned int>(row)));
                _start_position += Vector2f(_tile_size / 2.0f, _tile_size / 2.0f);
                break;
            case 'e':
                tile = END;
                break;
            case 'n':
                tile = ENEMY;
                break;
            case ' ':
                tile = EMPTY;
                break;
            case '+':
                tile = WAYPOINT;
                break;
            default:
                cerr << "Unknown tile character: '" << c << "' at (" << col << "," << row << ")" << endl;
                break;
            }

            _tiles[row * w + col] = tile;
        }
        row++;
    }

    f.close();

    // Load dirt texture
    _wall_texture = make_shared<sf::Texture>();
    if (_wall_texture->loadFromFile("../../../../resources/textures/dirt_sprite_not_derivative.png")) {
        _use_texture = true;
        cout << "DIRT SPRITE LOADED !!!" << endl;
    } else {
        cerr << "DIRT SPRITE LOAD ERROR" << endl;
        _use_texture = false;
        _wall_texture.reset();
    }

    cout << "Level loaded: " << path << " (" << w << "x" << h << ")" << endl;
}

void LevelSystem::unload_level() {
    _tiles.reset();
    _wall_texture.reset();
    _width = 0;
    _height = 0;
    _offset = Vector2f(0, 0);
    _use_texture = false;
}

void LevelSystem::render(RenderWindow& window) {
    if (!_tiles) {
        return;
    }

    for (size_t y = 0; y < _height; y++) {
        for (size_t x = 0; x < _width; x++) {
            Tile t = get_tile(Vector2u(static_cast<unsigned int>(x), static_cast<unsigned int>(y)));
            if (t == EMPTY) {
                continue;
            }

            Vector2f tile_pos = get_tile_position(Vector2u(static_cast<unsigned int>(x), static_cast<unsigned int>(y)));

            // puts textures on the walls
            if (t == WALL && _use_texture && _wall_texture) {
                Sprite dirt_sprite(*_wall_texture);
                dirt_sprite.setPosition(tile_pos);
                
                // Scale sprite to match tile size
                Vector2u tex_size = _wall_texture->getSize();
                dirt_sprite.setScale(_tile_size / tex_size.x, 
                                     _tile_size / tex_size.y);
                
                window.draw(dirt_sprite);
            } 
            // Fallback to solid colors for non-wall tiles or if texture failed
            else {
                RectangleShape rs(Vector2f(_tile_size, _tile_size));
                rs.setPosition(tile_pos);
                rs.setFillColor(get_color(t));
                window.draw(rs);
            }
        }
    }
}

LevelSystem::Tile LevelSystem::get_tile(Vector2u p) {
    if (p.x >= _width || p.y >= _height) {
        return EMPTY;
    }
    return _tiles[p.y * _width + p.x];
}

LevelSystem::Tile LevelSystem::get_tile(const Vector2i& p) {
    if (p.x < 0 || p.y < 0 || p.x >= static_cast<int>(_width) || p.y >= static_cast<int>(_height)) {
        return EMPTY;
    }
    return _tiles[p.y * _width + p.x];
}

LevelSystem::Tile LevelSystem::get_tile_at(const Vector2f& v) {
    auto a = v - _offset;
    if (a.x < 0 || a.y < 0) {
        return EMPTY;
    }
    const Vector2u p = {static_cast<unsigned int>(floor(a.x / _tile_size)),
                         static_cast<unsigned int>(floor(a.y / _tile_size))};
    return get_tile(p);
}

bool LevelSystem::is_on_grid(Vector2f v) {
    auto a = v - _offset;
    if (a.x < 0 || a.y < 0) {
        return false;
    }
    const Vector2u p = {static_cast<unsigned int>(floor(a.x / _tile_size)),
                         static_cast<unsigned int>(floor(a.y / _tile_size))};
    return !(p.x >= _width || p.y >= _height);
}

size_t LevelSystem::get_width() { return _width; }

size_t LevelSystem::get_height() { return _height; }

Vector2f LevelSystem::get_offset() { return _offset; }

void LevelSystem::set_offset(const Vector2f& offset) { _offset = offset; }

float LevelSystem::get_tile_size() { return _tile_size; }

Vector2f LevelSystem::get_tile_position(Vector2u p) {
    return (Vector2f(static_cast<float>(p.x), static_cast<float>(p.y)) * _tile_size) + _offset;
}

Vector2f LevelSystem::get_tile_position(const Vector2i& p) {
    return (Vector2f(static_cast<float>(p.x), static_cast<float>(p.y)) * _tile_size) + _offset;
}

vector<Vector2u> LevelSystem::find_tiles(Tile type) {
    vector<Vector2u> result;
    for (size_t y = 0; y < _height; y++) {
        for (size_t x = 0; x < _width; x++) {
            if (get_tile(Vector2u(static_cast<unsigned int>(x), static_cast<unsigned int>(y))) == type) {
                result.push_back({static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
            }
        }
    }
    return result;
}

bool LevelSystem::in_group(const Vector2i& tile, const vector<Vector2i>& group) {
    return find(group.begin(), group.end(), tile) != group.end();
}

vector<vector<Vector2i>> LevelSystem::get_groups(Tile type) {
    vector<vector<Vector2i>> groups;
    vector<Vector2i> all_tiles;

    // Find all tiles of the specified type
    for (int y = 0; y < static_cast<int>(_height); y++) {
        for (int x = 0; x < static_cast<int>(_width); x++) {
            if (get_tile(Vector2i(x, y)) == type) {
                all_tiles.push_back({x, y});
            }
        }
    }

    // Group adjacent tiles
    while (!all_tiles.empty()) {
        vector<Vector2i> group;
        vector<Vector2i> to_process;
        to_process.push_back(all_tiles.back());
        all_tiles.pop_back();

        while (!to_process.empty()) {
            Vector2i current = to_process.back();
            to_process.pop_back();
            group.push_back(current);

            // Check 4 adjacent tiles
            const Vector2i neighbors[] = {
                {current.x + 1, current.y},
                {current.x - 1, current.y},
                {current.x, current.y + 1},
                {current.x, current.y - 1}
            };

            for (const auto& neighbor : neighbors) {
                auto it = find(all_tiles.begin(), all_tiles.end(), neighbor);
                if (it != all_tiles.end()) {
                    to_process.push_back(*it);
                    all_tiles.erase(it);
                }
            }
        }

        groups.push_back(group);
    }

    return groups;
}

Color LevelSystem::get_color(Tile t) {
    auto it = _colors.find(t);
    if (it == _colors.end()) {
        return Color::Transparent;
    }
    return it->second;
}

void LevelSystem::set_color(Tile t, Color c) {
    _colors[t] = c;
}

Vector2f LevelSystem::get_start_position() {
    return _start_position;
}

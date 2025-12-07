#pragma once

#include <SFML/Audio.hpp>

class AudioSystem
{
public:
	void add_sound(char* file);
	void play_sound();
	sf::SoundSource::Status is_sound_playing();
	static const void load_music(char* file);
	static const void play_pause_music(bool pause);
	static const void stop_music();
	static const sf::SoundSource::Status is_music_playing();

	// Volume control 
	static void set_music_volume(float volume);
	static void set_sfx_volume(float volume);

protected:
	static std::vector<sf::SoundBuffer> _sound_buffers;
	static std::vector<sf::Sound> _sounds;
	static sf::Music _music;

	sf::Sound _sound;
	sf::SoundBuffer _buffer;
};
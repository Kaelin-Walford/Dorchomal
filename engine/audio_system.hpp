#pragma once

#include <SFML/Audio.hpp>

class AudioSystem
{
public:
	static const void add_sound_to_queue(char* file);
	static const void load_music(char* file);
	static const void play_pause_music(bool pause);
	static const void stop_music();
	static const void play_sounds_in_queue();
	static const sf::SoundSource::Status is_music_playing();
protected:
	static std::vector<sf::SoundBuffer> _sound_buffers;
	static std::vector<sf::Sound> _sounds;
	static sf::Music _music;
};
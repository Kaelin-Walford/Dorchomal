#include "audio_system.hpp"
#include <iostream>

/*const void AudioSystem::add_sound_to_queue(char* file)
{
	//adds new buffer to the end of the sound buffer
	//_sound_buffers;
	char path[100] = "../../../../resources/Sounds/Sound Effects/";
	strcat(path, file);
	sf::SoundBuffer sound_buffer;
	sound_buffer.loadFromFile(path);
	_sound_buffers.push_back(sound_buffer);


	//Adds the buffer sound to the end of the sound
	sf::Sound sounds;
	sounds.setBuffer(_sound_buffers[_sound_buffers.size() - 1]);
	//sounds.play();
	//_sounds.emplace_back();
	//_sounds.back().setBuffer(_sound_buffers.back());

	_sounds.push_back(sounds);
	_sounds[_sounds.size() - 1].play();
}*/

// Define static members
std::vector<sf::SoundBuffer> AudioSystem::_sound_buffers;
std::vector<sf::Sound> AudioSystem::_sounds;
sf::Music AudioSystem::_music;

void AudioSystem::add_sound(char* file)
{
	//adds new buffer to the end of the sound buffer
	char path[100] = "../../../../resources/Sounds/Sound Effects/";
	strcat(path, file);
	_buffer.loadFromFile(path);
	_sound.setBuffer(_buffer);
}

void AudioSystem::play_sound()
{
	_sound.play();
}

sf::SoundSource::Status AudioSystem::is_sound_playing()
{
	return _sound.getStatus();
}

const void AudioSystem::load_music(char* file)
{
	char path[100] = "../../../../resources/Sounds/Music/";
	strcat(path, file);
	_music.openFromFile(path);
	_music.setLoop(true);;
}

//Plays and pauses the music
const void AudioSystem::play_pause_music(bool pause)
{
	if (pause)
	{
		_music.play();
	}
	else
	{
		_music.pause();
	}
}

const void AudioSystem::stop_music()
{
	_music.stop();
}

const sf::SoundSource::Status AudioSystem::is_music_playing()
{
	return _music.getStatus();
}

void AudioSystem::set_music_volume(float volume)
{
	_music.setVolume(volume);
}

void AudioSystem::set_sfx_volume(float volume)
{
	for (auto& sound : _sounds)
	{
		sound.setVolume(volume);
	}
}
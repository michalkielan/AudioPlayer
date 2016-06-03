//============================================================================
// Name        : AudioBase.cpp
// Author      : 
// Version     :
// Copyright   : copyright alright
// Description : Playing sound with alsa library
//============================================================================

#include <iostream>
#include <regex>
#include "WavPlayer.h"


/**
 * \ Wav player paramters
 */
Audio::AudioParam wavParam {
	SND_PCM_STREAM_PLAYBACK, {}, {}, {}, 44
};


bool compareExtension(std::string& filename, std::string extension)
{
	std::string file_ext = filename.substr(filename.find_last_of(".") + 1);

	if(std::regex_match(file_ext, std::regex{extension, std::regex_constants::icase}))
		return true;
	else
		return false;
}


bool isWavExtension(std::string& filename)
{
	return compareExtension(filename, "wav");
}


int main(int argc, char* argv[])
{
	std::cout << "ALSA Aplication Test: " << SND_LIB_VERSION_STR << std::endl;

	// get file
	if(argc < 2)
	{
		std::cout << "No file arguments, please call like: ./player /dir/.../song.wav" << std::endl;
		return -1;
	}

	else if(argc > 2)
	{
		std::cout << "To many arguments" << std::endl;
		return -1;
	}

	std::string filename{argv[1]};
	auto* filenamep = argv[1];
	std::unique_ptr<Audio::Player> player;

	if(isWavExtension(filename))
		player = std::make_unique<Audio::WavPlayer>(Audio::WavPlayer{"default", wavParam});

	else
		player.reset();

	std::cout << "start playing" << std::endl;
		player.get()->play(filenamep);
	std::cout << "stop playing" << std::endl;


	std::cout << "start playing" << std::endl;
		player.get()->play(filenamep);
	std::cout << "stop playing" << std::endl;

	return 0;
}




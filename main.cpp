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
#include <chrono>


/**
 * \ Wav player paramters
 */
Audio::SoundParam wavParam { SND_PCM_STREAM_PLAYBACK, SND_PCM_ACCESS_RW_INTERLEAVED, {}, {}, 44 };


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




//tests
int test_unique_ptr_allocate(std::string filename)
{
	std::unique_ptr<Audio::Player> player;
	const char* name = filename.c_str();

	if(isWavExtension(filename))
		player = std::unique_ptr<Audio::WavPlayer>{new Audio::WavPlayer{"default", wavParam}};
	else
		return -1;

	player.get()->play(name);

	return 0;

}


int test_normal_allocate(std::string filename)
{
	Audio::Player *player = new Audio::WavPlayer{"default", wavParam};
	const char* name = filename.c_str();

	if(player != nullptr)
		player->play(name);

	else
		return -1;

	delete player;
	player = nullptr;

	return 0;
}


int test_normal_object(std::string filename)
{
	const char* name = filename.c_str();
	Audio::WavPlayer player {"default", wavParam};
	player.play(name);

	return 0;
}


void test_cpy_obj(std::string filename)
{
	Audio::WavPlayer p1 {"default", wavParam};
	Audio::WavPlayer p2 = p1;
	const char* name = filename.c_str();

	p1.play(name);
	p2.play(name);

	auto p3 { std::move(p1) };

	p3.play(name);
	p1.play(name);
}

/**
 *
 * \ Alsa training application
 *
 */
int main(int argc, char* argv[])
{

	std::cout << "ALSA Aplication Test: " << SND_LIB_VERSION_STR << std::endl;

	// no file
	if(argc < 2)
	{
		std::cout << "No file arguments, please call like: ./player /dir/.../song.wav" << std::endl;
		return -1;
	}

	// to many
	else if(argc > 2)
	{
		std::cout << "To many arguments" << std::endl;
		return -1;
	}

	// file name arg ok
	else
	{
		const char* filenamep = argv[1];
		std::string filename{filenamep};
		int err {};

		test_normal_allocate(filename);
		test_normal_object(filename);
		test_unique_ptr_allocate(filename);
		test_cpy_obj(filename);

		return 0;
	}
}






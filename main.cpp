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

#include <functional>
/**
 * \ Wav player paramters
 */
Audio::AudioParam wavParam {
	SND_PCM_STREAM_PLAYBACK, SND_PCM_ACCESS_RW_INTERLEAVED, {}, {}, 44
};

Audio::AudioParam recoredParam {
	SND_PCM_STREAM_CAPTURE, SND_PCM_ACCESS_RW_INTERLEAVED, SND_PCM_FORMAT_S16_LE, Audio::Channels::stereo, 44100
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




/*
 * replace function pointers
 */
using namespace std::placeholders;
class A
{
	std::once_flag chooseWriteFunction;
	std::function<void(char*, size_t)> f ;

public:
	int writeI(char* buf, size_t len)
	{
		std::cout <<"write i" << std::endl;
	}

	int writeNI(char* buf, size_t len)
	{
		std::cout <<"write ni" << std::endl;
	}

	void checkStuff()
	{

		f = [=](char* buf, size_t len) {
		    return this->writeNI(buf, len);
		};


		f = [=](char* buf, size_t len) {
		    return this->writeI(buf, len);
		};

	}

	void write(char * buf, size_t len)
	{
		//std::call_once(chooseWriteFunction, [](){ this->checkStuff(); });

		f(buf, len);
	}
};
//
//int main()
//{
//	char msg[] = "spafpfhefwe\0";
//	A a;
//
//	a.write(msg, strlen(msg));
//}


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

	auto* filenamep = argv[1];
	std::string filename{filenamep};
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

//	Audio::Recorder rec {"default", recoredParam};
//
//	auto data = rec.read(128);
//
//	for (int i = 0; i < 10; i++) {
//		//std::cout << recorded.first[i] - '0' << " ";
//		printf("%d ", data.first[i]);
//	}
//	return 0;


	exit(0);
}




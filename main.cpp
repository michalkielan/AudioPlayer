//============================================================================
// Name        : alsaProj1.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>

#include "WavPlayer.h"



/**
 * \ Wav player paramters
 */
Audio::AudioParam wavParam {
	SND_PCM_STREAM_PLAYBACK, {}, {}, {}, 44
};



int main(int argc,char *argv[])
{
	std::cout << "ALSA Aplication Test: " << SND_LIB_VERSION_STR << std::endl;

	std::unique_ptr<Audio::Player> player;

	if(1)	// is wav file
		player = std::make_unique<Audio::WavPlayer>(Audio::WavPlayer{"default", wavParam});

	else
		player.reset();


	player.get()->play("/home/michal/Workspace/WavTest/Roland-JD-990-Windchimes.wav");


	return 0;
}




























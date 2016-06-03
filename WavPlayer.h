/*
 * SoundWav.h
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#ifndef WAVPLAYER_H_
#define WAVPLAYER_H_

#include "Player.h"
#include <memory>
#include <fstream>
#include "AudioBase.h"

namespace Audio {

#pragma pack (1)



// IFF file header
typedef struct WavFileHeader
{
	/**
	 * \ RIFF or FORM
	 */
	char	id[4];


	/**
	 * \ Length of subsequent file (including remainder of header). This is
	 * \ in Intel reverse byte order if RIFF, Motorola format if FORM.
	 */
	unsigned int	len;


	/**
	 * \ WAVE or AIFF
	 */
	char	type[4];
} WavFileHeader;



// IFF chunk header
typedef struct WavChunkHeader
{
	/**
	 * Chunk id
	 */
	char id[4];

	/* Length of subsequent data within this chunk. this is in Intel reverse byte
	 * order if RIFF, Motorola format if FORM. Note: this doesn't include any
	 * extra byte needed to pad the chunk out to an even size.
	 */
	unsigned int  len;

} WavChunkHeader;



// WAVE fmt chunk
typedef struct WavFmtHeader {
	short			audioFormat;
	unsigned short	channels;
	unsigned int	sampleRate;
	unsigned int	byteRate;
	unsigned short	blockAlign;
	unsigned short	bitsPerSample;
} WavFmtHeader;

#pragma pack()




/**
 * \ Structure with alsa library paramters
 */
typedef struct SoundParam {
	snd_pcm_stream_t stream;
	snd_pcm_access_t access;
	snd_pcm_format_t format;
	Channels channels;
	unsigned int rate;

	SoundParam(
		snd_pcm_stream_t _stream,
		snd_pcm_access_t _access,
		snd_pcm_format_t _format,
		Channels _channels,
		unsigned int _rate) :
	stream{_stream},
	access{_access},
	format {_format},
	channels {_channels},
	rate {_rate}
	{

	};

} AudioParam;


/**
 * \ Buffer with smart pointer and size
 */
typedef struct Buffer
{
	std::unique_ptr<char[]> data;
	size_t len {};
} Buffer;


class WavPlayer : public Player, public AudioBase
{
	snd_pcm_uframes_t waveSize {};
	WavFmtHeader fmt {};

	const char *filename { nullptr };
	Buffer buf {nullptr, 0};
	size_t size {};


	/**
	 * \ Check the format of wav file is correct
	 */
	bool isWavFile(std::fstream& file);


	/**
	 * \ Convert unsigned char to enumerated format type
	 */
	snd_pcm_format_t convertBitsToPcmFormat(const unsigned char bits);


	/**
	 * \ load file method
	 */
	void load(const char* _filename);



public:

	/**
	 * \ Constructor with file name and audio parameters
	 */
	WavPlayer(const char *_name, SoundParam param);


	/**
	 * \ play method overloaded from player interface
	 */
	void play(const char* _filename) override;
};

} /* namespace Audio */

#endif /* WAVPLAYER_H_ */

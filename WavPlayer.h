/*
 * SoundWav.h
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#ifndef WAVPLAYER_H_
#define WAVPLAYER_H_

#include <memory>
#include <fstream>

#include "Player.h"
#include "PcmInterface.h"

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

} SoundParam;


/**
 * \ Buffer with smart pointer and size
 */
template<typename T>
class Buffer
{
public:
	std::unique_ptr<T[]> data;
	size_t len {};


	Buffer(T* p, size_t _len) : data {p}, len{_len}
	{
	}

	Buffer() : data {nullptr}, len{0}
	{
	}

	Buffer(const Buffer& b) : Buffer{}
	{
	}

	~Buffer()
	{
		len = 0;
		data = nullptr;
	}
};


class WavPlayer : public Player, public PcmInterface
{
	snd_pcm_uframes_t waveSize {};
	WavFmtHeader fmt {};
	SoundParam param;

	const char *filename { nullptr };
	Buffer<char> buf {nullptr, 0};
	size_t size {};


	/**
	 * \Check if is *.wav file using wav file header
	 */
	bool isWavFile(std::ifstream& file);
	/**
	 * \ Bits in num ti snd_pcm_format
	 */
	snd_pcm_format_t convertBitsToPcmFormat(const unsigned char bits);


	/**
	 * \ Load file
	 */
	void load(const char* _filename);


	/**
	 * \ Open device and allocate different data
	 */
	void open();


	/**
	 * \ Close device and free memory
	 */
	void close();


public:

	/**
	 * \ Constructor call open methods
	 */
	WavPlayer(const char *_name, SoundParam _param) ;


	/**
	 * \ Copy constructor call, default constructor
	 */
	WavPlayer(const WavPlayer& wp);


	/**
	 * \ Equal operator overloaded
	 */
	WavPlayer& operator=(const WavPlayer& wp);


	/**
	 * \ Destructor close device and deallocate the memory
	 */
	~WavPlayer();


	/**
	 * \ Play using the buffer
	 */
	void play(const char* _filename) override;

};

} /* namespace Audio */

#endif /* WAVPLAYER_H_ */

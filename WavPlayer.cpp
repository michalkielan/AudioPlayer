/*
 * SoundWav.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#include "WavPlayer.h"

namespace Audio {


WavPlayer::WavPlayer(const char *_name, AudioParam param) : AudioBase{ _name }
{
	opendev(param.stream);
}


bool WavPlayer::isWavFile(std::fstream& file)
{
	WavFileHeader fileHeader {};

	// check if data size is correct
	if(file.read(reinterpret_cast<char *>(&fileHeader), sizeof(WavFileHeader)))
	{
		// check if format is RIFF or FORM
		const auto id = std::string{fileHeader.id,4};
		if(!(id == "RIFF" || id == "FORM"))
			return false;

		// check if type is WAVE or AIFF
		const auto type = std::string{fileHeader.type,4};
		if(!(type == "WAVE" || type == "AIFF"))
			return false;

		buf.len = fileHeader.len;
		return true;
	}

	else
		return false;
}


void WavPlayer::load(const char* _filename)
{
	std::fstream fs;
	WavChunkHeader chunkHeader {};
	filename = _filename;

	fs.open(filename, std::fstream::in);

	if(!isWavFile(fs))
		abort();

	while (fs.read(reinterpret_cast<char *>(&chunkHeader), sizeof(WavChunkHeader)))
	{
		const auto id = std::string{chunkHeader.id,4};

		if(id == "fmt ")
		{
			WavFmtParam fmt;

			if(fs.read(reinterpret_cast<char *>(&fmt), sizeof(WavFmtParam)))
			{
				if(fmt.wFormatTag != 1)
				{
					log_writeln("Compressed WAVE not supported yet");
					abort();
				}
			}

			else
				break;

			waveBits = static_cast<unsigned char>(fmt.wBitsPerSample);
			waveRate = static_cast<unsigned short>(fmt.dwSamplesPerSec);
			waveChannels = static_cast<Channels>(fmt.wChannels);
		}

		else if (id == "data")
		{
			buf.data = std::make_unique<char[]>(buf.len);

			if(buf.data.get() == nullptr)
			{
				log_writeln("Allocate memory error");
				abort();
			}

			const auto offsite { 100 };
			fs.read(buf.data.get(), buf.len - offsite);	/// TODO - wtf with len

			if(fs)
				waveSize = (buf.len * 8) / ( static_cast<unsigned int>(waveBits) * static_cast<unsigned int>(waveChannels) );

			else
			{
				log_writeln( "Error read, only " + std::to_string(fs.gcount()) + " could be read");
				break;
			}
		}
	}

	setParams(convertBitsToPcmFormat(waveBits), SND_PCM_ACCESS_RW_INTERLEAVED, waveChannels, waveRate, 1, 500000);
}


snd_pcm_format_t WavPlayer::convertBitsToPcmFormat(const unsigned char bits)
{
	snd_pcm_format_t format;

	switch (bits)
	{
		case 8:
			format = SND_PCM_FORMAT_U8;
			break;

		case 16:
			format = SND_PCM_FORMAT_S16;
			break;

		case 24:
			format = SND_PCM_FORMAT_S24;
			break;

		case 32:
			format = SND_PCM_FORMAT_S32;
			break;

		default:
			format = SND_PCM_FORMAT_UNKNOWN;
	}

	return format;
}


void WavPlayer::play(const char* _filename)
{
	snd_pcm_uframes_t count {}, frames {};

	load( _filename);

	if(buf.data.get() != nullptr)
	{
		do
		{
			frames = writeInterleaved(buf.data.get() + count, waveSize - count);
			count += frames;

		} while (count < waveSize);

		// Wait for playback to completely finish
		if (count == waveSize)
			stopPresentingFrames();
	}

	else
		log_write("File not loaded correctly");

}



} /* namespace Sound */

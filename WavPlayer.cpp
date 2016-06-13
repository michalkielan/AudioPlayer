/*
 * SoundWav.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#include "WavPlayer.h"

namespace Audio {


WavPlayer::WavPlayer(const char *_name, SoundParam _param) : PcmInterface{ _name }, param{_param}
{
	open();

	if(!pcm->isOpen())
		abort();
}


WavPlayer::WavPlayer(const WavPlayer& wp) : WavPlayer{wp.getName().c_str(), wp.param}
{
}


WavPlayer& WavPlayer::operator=(const WavPlayer& wp)
{
	param = wp.param;
    return *this;
}


WavPlayer::~WavPlayer()
{
	close();
}


void WavPlayer::open()
{
	pcm->opendev(param.stream);
	pcm->hwParamAllocateDefault();
}


void WavPlayer::close()
{
	pcm->hwParamFree();
	pcm->closedev();
}


bool WavPlayer::isWavFile(std::ifstream& file)
{
	WavFileHeader fileHeader {};

	// check if data size is correct
	if(file.read(reinterpret_cast<char *>(&fileHeader), sizeof(WavFileHeader)))
	{
		const auto id = std::string{fileHeader.id,4};
		const auto type = std::string{fileHeader.type,4};

		// check if format is RIFF or FORM
		if(!(id == "RIFF" || id == "FORM"))
			return false;

		// check if type is WAVE or AIFF
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
	std::ifstream fs;
	WavChunkHeader chunkHeader {};
	filename = _filename;

	fs.open(filename, std::ios::binary);

	if(!fs.is_open())
		abort();

	if(!isWavFile(fs))
		abort();

	while (fs.read(reinterpret_cast<char *>(&chunkHeader), sizeof(WavChunkHeader)))
	{
		const auto id = std::string{chunkHeader.id, 4};

		if(id == "fmt ")
		{
			if(fs.read(reinterpret_cast<char *>(&fmt), sizeof(WavFmtHeader)))
			{
				if(fmt.audioFormat != 1)
				{
					log_writeln("Compressed WAVE not supported yet");
					abort();
				}
			}

			else
				break;
		}

		else if (id == "data")
		{
			buf.data = std::make_unique<char[]>(buf.len);

			if(buf.data.get() == nullptr)
			{
				log_writeln("Allocate memory error");
				abort();
			}

			fs.seekg(0, std::ios::beg);
			fs.read(buf.data.get(), buf.len);

			if(fs)
				waveSize = (buf.len * 8) / ( static_cast<unsigned int>(fmt.bitsPerSample) * static_cast<unsigned int>(fmt.channels) );

			else
			{
				log_writeln( "Error read, only " + std::to_string(fs.gcount()) + " could be read");
				break;
			}
		}
	}

	pcm->setParams(convertBitsToPcmFormat(fmt.bitsPerSample), param.access, static_cast<Channels>(fmt.channels),
			static_cast<unsigned short>(fmt.byteRate), Resample::allow, std::chrono::milliseconds{ 500 });
}


snd_pcm_format_t WavPlayer::convertBitsToPcmFormat(const unsigned char bits)
{
	snd_pcm_format_t format {};

	switch (bits)
	{
		case 8:  format = SND_PCM_FORMAT_U8;  	  break;
		case 16: format = SND_PCM_FORMAT_S16; 	  break;
		case 24: format = SND_PCM_FORMAT_S24; 	  break;
		case 32: format = SND_PCM_FORMAT_S32; 	  break;
		default: format = SND_PCM_FORMAT_UNKNOWN; break;
	}

	return format;
}


void WavPlayer::play(const char* _filename)
{
	snd_pcm_uframes_t count {}, frames { };

	load(_filename);

	if ((buf.data.get() != nullptr) && (waveSize != 0))
	{
		do
		{
			frames = pcm->write(buf.data.get() + count, waveSize - count);
			count += frames;

		} while (count < waveSize);

		// Wait for playback to completely finish
		if (count >= waveSize)
			pcm->stopPresentingFrames();
	}

	else
		log_write("Wave not loaded correctly");
}


} /* namespace Audio */

/*
 * SoundBase.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#include "AudioBase.h"

namespace Audio {

AudioBase::AudioBase(const char* _name) : name{_name}
{

}


void AudioBase::abort(int err)
{
	log_write( "Error: ");
	log_writeln(snd_strerror(err) );
	throw std::string(snd_strerror(err));
}


void AudioBase::opendev(snd_pcm_stream_t stream, int mode)
{
	int err = snd_pcm_open(&soundDevice, name, stream, mode);
	if(err < 0)
		abort(err);

	allocateDefault();
}


void AudioBase::allocateDefault()
{
	int err = snd_pcm_hw_params_malloc(&hwParams);
	if(err < 0)
		abort(err);

	err = snd_pcm_hw_params_any(soundDevice, hwParams);
	if(err < 0)
		abort(err);
}


void AudioBase::setAccess(snd_pcm_access_t access)
{
	int err = snd_pcm_hw_params_set_access(soundDevice, hwParams, access);
	if(err < 0)
		abort(err);
}


void AudioBase::setFormat(snd_pcm_format_t format)
{
	int err = snd_pcm_hw_params_set_format(soundDevice, hwParams, format);
	if(err < 0)
		abort(err);
}


void AudioBase::setChannels(Channels channels)
{
	int err = snd_pcm_hw_params_set_channels(soundDevice, hwParams, static_cast<unsigned int>(channels));
	if(err < 0)
		abort(err);
}


void AudioBase::setRateNear(unsigned int val, int dir)
{
	int err = snd_pcm_hw_params_set_rate_near(soundDevice, hwParams, &val, &dir);
	if(err < 0)
		abort(err);
}


void AudioBase::setParams(snd_pcm_format_t format, snd_pcm_access_t access, Channels channels,
		unsigned int rate, int soft_resample, unsigned int latency)
{
	int err = snd_pcm_set_params(soundDevice, format, access,
				static_cast<unsigned int>(channels), rate, soft_resample, latency);
	if(err < 0)
		abort(err);
}


snd_pcm_sframes_t AudioBase::writeInterleaved(const void* buffer, snd_pcm_uframes_t size)
{
	int frames = snd_pcm_writei(soundDevice, buffer, size);

	if(frames < 0)
		recoverStream(frames, 0);

	return frames;
}


snd_pcm_sframes_t AudioBase::writeNonInterleaved(void** buffer, snd_pcm_uframes_t size)
{
	int frames = snd_pcm_writen(soundDevice, buffer, size);

	if(frames < 0)
		recoverStream(frames, 0);

	return frames;
}

snd_pcm_sframes_t AudioBase::write(void* buffer, snd_pcm_uframes_t size)
{
	auto access = getAccess();
	snd_pcm_sframes_t frames {};

	if( (access == SND_PCM_ACCESS_MMAP_INTERLEAVED) ||
	(access == SND_PCM_ACCESS_RW_INTERLEAVED) )
		frames =writeInterleaved(buffer, size);

	else if ( (access == SND_PCM_ACCESS_RW_NONINTERLEAVED) ||
	(access == SND_PCM_ACCESS_MMAP_NONINTERLEAVED) )
		frames = writeNonInterleaved(&buffer, size);

	else
		abort();

	return frames;
}


void AudioBase::recoverStream(int err, int silent)
{
	int _err = snd_pcm_recover(soundDevice, err, silent);
	if(_err < 0)
		abort();
}


void AudioBase::stopPresentingFrames()
{
	int err = snd_pcm_drain(soundDevice);
	if(err < 0)
		abort();
}


void AudioBase::start()
{
	int err = snd_pcm_hw_params(soundDevice, hwParams);
	if(err < 0)
		abort(err);
}


std::string AudioBase::getName() const
{
	return std::string{ snd_pcm_name(soundDevice) };
}


std::string AudioBase::getState() const
{
	return std::string{ snd_pcm_state_name(snd_pcm_state(soundDevice)) };
}


Channels AudioBase::getChannels() const
{
	unsigned int channels {};
	snd_pcm_hw_params_get_channels(hwParams, &channels);
	return static_cast<Channels>(channels);
}


unsigned int AudioBase::getRate() const
{
	unsigned int rate {};
	snd_pcm_hw_params_get_rate(hwParams, &rate, 0);
	return rate;
}


snd_pcm_access_t AudioBase::getAccess()
{
	snd_pcm_access_t access {};
	snd_pcm_hw_params_get_access(hwParams, &access);

	if(!((access >= 0) && (access <= SND_PCM_ACCESS_LAST)))
		abort(static_cast<int>(access));

	return access;
}


/*
 *
 */
AudioBase::~AudioBase()
{

}

} /* namespace Audio */

/*
 * SoundBase.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#include "PcmInterface.h"

namespace Audio {

PcmInterface::PcmInterface(const char* _name) : name{_name}
{
	if(name == nullptr)
		abort();
}


char* PcmInterface::decode_err(int err)
{
	return strerror(err);
}


const char* PcmInterface::decode_snd_err(int err)
{
	return snd_strerror(err);
}


void PcmInterface::abort(const char* decodedError)
{
	log_write("Error: ");
	log_writeln(decodedError);
	throw std::string{decodedError};
}


void PcmInterface::abort()
{
	throw std::string{""};
}


void PcmInterface::opendev(const snd_pcm_stream_t stream, const OpenMode mode)
{
	int err = snd_pcm_open(&soundDevice, name, stream, static_cast<int>(mode));
	if(err < 0)
		abort(decode_snd_err(err));
}


void PcmInterface::closedev()
{
	int err = snd_pcm_close(soundDevice);
	if(err < 0)
		abort(decode_snd_err(err));
	soundDevice = nullptr;
}


void PcmInterface::hwParamAllocateDefault()
{
	int err = snd_pcm_hw_params_malloc(&hwParams);
	if(err < 0)
		abort(decode_snd_err(err));

	err = snd_pcm_hw_params_any(soundDevice, hwParams);
	if(err < 0)
		abort(decode_snd_err(err));
}


void PcmInterface::hwParamFree()
{
	snd_pcm_hw_params_free(hwParams);
	hwParams = nullptr;
}


void PcmInterface::setAccess(const snd_pcm_access_t access)
{
	int err = snd_pcm_hw_params_set_access(soundDevice, hwParams, access);
	if(err < 0)
		abort(decode_snd_err(err));
}


void PcmInterface::setFormat(const snd_pcm_format_t format)
{
	int err = snd_pcm_hw_params_set_format(soundDevice, hwParams, format);
	if(err < 0)
		abort(decode_snd_err(err));
}


void PcmInterface::setChannels(const Channels channels)
{
	int err = snd_pcm_hw_params_set_channels(soundDevice, hwParams, static_cast<unsigned int>(channels));
	if(err < 0)
		abort(decode_snd_err(err));
}


void PcmInterface::setRateNear(unsigned int val, int dir)
{
	int err = snd_pcm_hw_params_set_rate_near(soundDevice, hwParams, &val, &dir);
	if(err < 0)
		abort(decode_snd_err(err));
}


bool PcmInterface::isInterleaved()
{
	const auto access = getAccess();
	bool status {};

	if( (access == SND_PCM_ACCESS_MMAP_INTERLEAVED) ||
	(access == SND_PCM_ACCESS_RW_INTERLEAVED) )
		status = true;

	else if ( (access == SND_PCM_ACCESS_RW_NONINTERLEAVED) ||
	(access == SND_PCM_ACCESS_MMAP_NONINTERLEAVED) )
		status = false;

	else
		abort();

	return status;
}


snd_pcm_sframes_t PcmInterface::writeInterleaved(const void* buffer, const snd_pcm_uframes_t size, const Silent silent)
{
	int frames = snd_pcm_writei(soundDevice, buffer, size);

	if(frames < 0)
		recoverStream(frames, silent);

	return frames;
}


snd_pcm_sframes_t PcmInterface::writeNonInterleaved(void* buffer, const snd_pcm_uframes_t size, const Silent silent)
{
	int frames = snd_pcm_writen(soundDevice, &buffer, size);

	if(frames < 0)
		recoverStream(frames, silent);

	return frames;
}


snd_pcm_sframes_t PcmInterface::write(void* buffer, const snd_pcm_uframes_t size)
{
	std::function<snd_pcm_sframes_t(void*, size_t)> writeFunctor { nullptr };

	if (isInterleaved())
		writeFunctor = [=](void* buf, size_t len) { return this->writeInterleaved(buf, len); };

	else
		writeFunctor = [=](void* buf, size_t len) { return this->writeNonInterleaved(&buf, len); };

	return writeFunctor(buffer, size);
}


void PcmInterface::recoverStream(const int err, const Silent silent)
{
	int _err = snd_pcm_recover(soundDevice, err, static_cast<int>(silent));
	if(_err < 0)
		abort();
}


void PcmInterface::stopPresentingFrames()
{
	int err = snd_pcm_drain(soundDevice);
	if(err < 0)
		abort();
}


void PcmInterface::setParam()
{
	int err = snd_pcm_hw_params(soundDevice, hwParams);
	if(err < 0)
		abort(decode_snd_err(err));
}


std::string PcmInterface::getName() const
{
	return std::string{ snd_pcm_name(soundDevice) };
}


std::string PcmInterface::getStateName() const
{
	return std::string{ snd_pcm_state_name(snd_pcm_state(soundDevice)) };
}


bool PcmInterface::isOpen() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_OPEN ? true : false);
}


bool PcmInterface::isSetup() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_SETUP ? true : false);;
}


bool PcmInterface::isPrepared() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_PREPARED ? true : false);
}


bool PcmInterface::isRunning() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_RUNNING ? true : false);
}


bool PcmInterface::isStateXrun() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_XRUN ? true : false);
}


bool PcmInterface::isDraining() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_DRAINING ? true : false);
}


bool PcmInterface::isPaused() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_PAUSED ? true : false);
}


bool PcmInterface::isSuspended() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_SUSPENDED ? true : false);
}


bool PcmInterface::isDisconnected() const
{
	auto status = snd_pcm_state(soundDevice);
	return (status == snd_pcm_state_t::SND_PCM_STATE_DISCONNECTED ? true : false);
}




Channels PcmInterface::getChannels() const
{
	unsigned int channels {};
	snd_pcm_hw_params_get_channels(hwParams, &channels);
	return static_cast<Channels>(channels);
}


unsigned int PcmInterface::getRate() const
{
	unsigned int rate {};
	snd_pcm_hw_params_get_rate(hwParams, &rate, 0);
	return rate;
}


snd_pcm_access_t PcmInterface::getAccess()
{
	snd_pcm_access_t access {};
	snd_pcm_hw_params_get_access(hwParams, &access);

	if(!((access >= 0) && (access <= SND_PCM_ACCESS_LAST)))
		abort(decode_snd_err(static_cast<int>(access)));

	return access;
}


PcmInterface::~PcmInterface()
{
}

} /* namespace Audio */

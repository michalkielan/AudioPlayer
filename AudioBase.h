/*
 * SoundBase.h
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#ifndef AUDIOBASE_H_
#define AUDIOBASE_H_

#include <alsa/asoundlib.h>
#include "Logger.h"
#include <iostream>


namespace Audio {

enum class Channels : unsigned int
{
	mono = 1,
	stereo = 2
};

/**
 * \ Base class with all configurations methods
 */
class AudioBase
{
//	std::shared_ptr<snd_pcm_t *> soundDevice;
//	std::shared_ptr<snd_pcm_hw_params_t *> hwParams;
	snd_pcm_t* soundDevice { nullptr };
	snd_pcm_hw_params_t* hwParams { nullptr };
	const char* name { nullptr };

public:

	AudioBase(const char* _name);

	snd_pcm_hw_params_t* getParamStructure() const;


	snd_pcm_t* getDeviceStructure() const;


	/**
	 * \ Abort function, make extension if error code is existing
	 */
	void abort(int err = -1);

	/**
	 * \ Open device
	 */
	void opendev(snd_pcm_stream_t stream, int mode = 0);

	/**
	 * \ Allocate hardware param memory and set the default values
	 */
	void allocateDefault();

	/**
	 * \
	 */
	void setAccess(snd_pcm_access_t access);


	/**
	 * \
	 */
	void setFormat(snd_pcm_format_t format);


	/**
	 * \
	 */
	void setChannels(Channels channels);


	/**
	 * \
	 */
	void setRateNear(unsigned int val, int dir = 0);

	/**
	 * \
	 */
	void setParams(snd_pcm_format_t format, snd_pcm_access_t access, Channels channels,
			unsigned int rate, int soft_resample, unsigned int latency);


	/**
	 * \
	 */
	snd_pcm_sframes_t writeInterleaved(const void* buffer, snd_pcm_uframes_t size);


	/**
	 * \
	 */
	snd_pcm_sframes_t writeNonInterleaved(void** buffer, snd_pcm_uframes_t size);


	/**
	 * \
	 */
	void recoverStream(int err, int silent);


	/**
	 * \
	 */
	void recoverStream(snd_pcm_t *pcm, int err, int silent);


	/**
	 * \
	 */
	void stopPresentingFrames();


	/**
	 * \ Write all parameters
	 */
	void start();


	/**
	 * \
	 */
	std::string getName() const;


	/**
	 * \
	 */
	std::string getState() const;


	/**
	 * \
	 */
	Channels getChannels() const;


	/**
	 * \
	 */
	unsigned int getRate() const;


	/**
	 * \
	 */
	snd_pcm_access_t getAccess() const;

	/*
	 *
	 */
	virtual ~AudioBase();
};

} /* namespace Sound */

#endif /* AUDIOBASE_H_ */

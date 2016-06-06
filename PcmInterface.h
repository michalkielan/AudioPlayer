/*
 * SoundBase.h
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#ifndef PCMINTERFACE_H_
#define PCMINTERFACE_H_

#include <alsa/asoundlib.h>
#include "Logger.h"
#include <iostream>
//#include <thread>
#include <mutex>
#include <memory>

namespace Audio {


enum class Channels : unsigned int
{
	mono = 1,
	stereo = 2
};


/**
 * \ Base class with all configurations methods
 */
class PcmInterface
{
	snd_pcm_t* soundDevice { nullptr };
	snd_pcm_hw_params_t* hwParams { nullptr };
	const char* name { nullptr };

	//std::once_flag chooseWriteFunction;
	std::function<snd_pcm_sframes_t(void*, size_t)> writeFunctor;

	bool isInterleaved();


public:

	PcmInterface *pcm {this};

	void functionChooser();
	auto* getDevice()
	{
		return soundDevice;
	}

	auto* getParam()
	{
		return hwParams;
	}

	PcmInterface(const char* _name);


	/**
	 * \ Abort functions, make extension if error code is existing
	 */
	char* decode_err(int err);
	const char* decode_snd_err(int err);

	void abort(const char* decodedError);
	void abort();



	/**
	 * \ Open device
	 */
	void opendev(snd_pcm_stream_t stream, int mode = 0);


	/**
	 * \ Allocate hardware param memory and set the default values
	 */
	void paramsAllocateDefault();


	/**
	 * \ Deallocate hardware param memory and set the default values
	 */
	void paramsFree();


	/**
	 * \ Prepare
	 */
	void prepare();


	/**
	 * \
	 */
	int getFormatWidth(snd_pcm_format_t format);


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
	snd_pcm_sframes_t writeNonInterleaved(void* buffer, snd_pcm_uframes_t size);


	/**
	 * \ Write
	 */
	snd_pcm_sframes_t write(void* buffer, snd_pcm_uframes_t size);


	/**
	 * \
	 */
	snd_pcm_sframes_t readInterleaved(void* buffer, snd_pcm_uframes_t size);


	/**
	 * \
	 */
	snd_pcm_sframes_t readNonInterleaved(void* buffer, snd_pcm_uframes_t size);


	/**
	 * \ Read
	 */
	snd_pcm_sframes_t read(void* buffer, snd_pcm_uframes_t size);


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
	void setParam();


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
	snd_pcm_access_t getAccess();


	/**
	 * \
	 */
	void close();


	/*
	 *
	 */
	virtual ~PcmInterface();
};



} /* namespace Audio */

#endif /* PCMINTERFACE_H_ */

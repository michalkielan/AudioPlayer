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
#include <memory>
#include <mutex>
#include <chrono>


namespace Audio {

enum class OpenMode : int
{
	block = 0,
	nonblock = SND_PCM_NONBLOCK	,
};


enum class Silent : int
{
	print_err_msg,
	do_not_print_err_msg
};


enum class Channels : unsigned int
{
	mono = 1,
	stereo = 2
};


enum class Resample : int
{
	disallow,
	allow
};

/**
 * \ Base class with all configurations methods
 */
class PcmInterface
{
	snd_pcm_t* soundDevice { nullptr };
	snd_pcm_hw_params_t* hwParams { nullptr };
	const char* name { nullptr };

	/*
	 * \ Pointer to write method
	 */
//	std::function<snd_pcm_sframes_t(void*, size_t)> writeFunctor { nullptr };

	bool isInterleaved();

	/**
	 * \
	 */
	void writeMethodChooser();

public:

	PcmInterface *pcm {this};

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
	void opendev(const snd_pcm_stream_t stream, const OpenMode mode = OpenMode::block);
	void closedev();

	/**
	 * \ Allocate hardware param memory and set the default values
	 */
	void hwParamAllocateDefault();
	void hwParamFree();


	/**
	 * \
	 */
	void setAccess(const snd_pcm_access_t access);


	/**
	 * \
	 */
	void setFormat(const snd_pcm_format_t format);


	/**
	 * \
	 */
	void setChannels(const Channels channels);


	/**
	 * \
	 */
	void setRateNear(unsigned int val, int dir = 0);


	/**
	 * \ Set all parameters method
	 */
	template <typename Rep, typename Period>
	void setParams(const snd_pcm_format_t format, const snd_pcm_access_t access, const Channels channels,
			const unsigned int rate, const Resample soft_resample, const std::chrono::duration<Rep, Period> latency)
	{
		const unsigned int latency_us = std::chrono::duration_cast<std::chrono::microseconds>(latency).count();

		int err = snd_pcm_set_params(soundDevice, format, access, static_cast<unsigned int>(channels),
				rate, static_cast<int>(soft_resample), latency_us);
		if(err < 0)
			abort(decode_snd_err(err));
	}


	/**
	 * \
	 */
	snd_pcm_sframes_t writeInterleaved(const void* buffer, const snd_pcm_uframes_t size, const Silent silent = Silent::print_err_msg);


	/**
	 * \
	 */
	snd_pcm_sframes_t writeNonInterleaved(void* buffer, const snd_pcm_uframes_t size, const Silent silent = Silent::print_err_msg);


	/**
	 * \ Write
	 */
	snd_pcm_sframes_t write(void* buffer, const snd_pcm_uframes_t size);

	/**
	 * \
	 */
	void recoverStream(int err, const Silent silent);


	/**
	 * \
	 */
	void recoverStream(snd_pcm_t *pcm, const int err, const int silent);


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
	std::string getStateName() const;


	/**
	 * \Read actual status
	 */
	bool isOpen() const;
	bool isSetup() const;
	bool isPrepared() const;
	bool isRunning() const;
	bool isStateXrun() const;
	bool isDraining() const;
	bool isPaused() const;
	bool isSuspended() const;
	bool isDisconnected() const;



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

	/*
	 *
	 */
	virtual ~PcmInterface();
};

} /* namespace Audio */

#endif /* PCMINTERFACE_H_ */

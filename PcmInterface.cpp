/*
 * SoundBase.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: michal
 */

#include "PcmInterface.h"

namespace Audio
{

PcmInterface::PcmInterface(const char* _name) : name{ _name }
{

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
  throw std::string(decodedError);
}

void PcmInterface::abort()
{
  throw std::string("");
}

void PcmInterface::opendev(snd_pcm_stream_t stream, int mode)
{
  int err = snd_pcm_open(&soundDevice, name, stream, mode);
  if (err < 0)
    abort(decode_snd_err(err));
}

void PcmInterface::paramsAllocateDefault()
{
  int err = snd_pcm_hw_params_malloc(&hwParams);
  if (err < 0)
    abort(decode_snd_err(err));

  err = snd_pcm_hw_params_any(soundDevice, hwParams);
  if (err < 0)
    abort(decode_snd_err(err));
}

void PcmInterface::paramsFree()
{
  snd_pcm_hw_params_free(hwParams);
}

void PcmInterface::prepare()
{
  int err = snd_pcm_prepare(soundDevice);
  if (err < 0)
    abort(decode_snd_err(err));
}

int PcmInterface::getFormatWidth(snd_pcm_format_t format)
{
  int width = snd_pcm_format_width(format);
  if (width < 0)
    abort(decode_snd_err(width));

  return width;
}

void PcmInterface::setAccess(snd_pcm_access_t access)
{
  int err = snd_pcm_hw_params_set_access(soundDevice, hwParams, access);
  if (err < 0)
    abort(decode_snd_err(err));
}

void PcmInterface::setFormat(snd_pcm_format_t format)
{
  int err = snd_pcm_hw_params_set_format(soundDevice, hwParams, format);
  if (err < 0)
    abort(decode_snd_err(err));
}

void PcmInterface::setChannels(Channels channels)
{
  int err = snd_pcm_hw_params_set_channels(soundDevice, hwParams,
      static_cast<unsigned int>(channels));
  if (err < 0)
    abort(decode_snd_err(err));
}

void PcmInterface::setRateNear(unsigned int val, int dir)
{
  int err = snd_pcm_hw_params_set_rate_near(soundDevice, hwParams, &val, &dir);
  if (err < 0)
    abort(decode_snd_err(err));
}

void PcmInterface::setParams(snd_pcm_format_t format, snd_pcm_access_t access,
    Channels channels, unsigned int rate, int soft_resample,
    unsigned int latency)
{
  int err = snd_pcm_set_params(soundDevice, format, access,
      static_cast<unsigned int>(channels), rate, soft_resample, latency);
  if (err < 0)
    abort(decode_snd_err(err));
}

snd_pcm_sframes_t PcmInterface::writeInterleaved(const void* buffer,
    snd_pcm_uframes_t size)
{
  int frames = snd_pcm_writei(soundDevice, buffer, size);

  if (frames < 0)
    recoverStream(frames, 0);

  return frames;
}

snd_pcm_sframes_t PcmInterface::writeNonInterleaved(void* buffer,
    snd_pcm_uframes_t size)
{
  int frames = snd_pcm_writen(soundDevice, &buffer, size);

  if (frames < 0)
    recoverStream(frames, 0);

  return frames;
}

bool PcmInterface::isInterleaved()
{
  auto access = getAccess();
  bool status{};

  if ((access == SND_PCM_ACCESS_MMAP_INTERLEAVED) || (access == SND_PCM_ACCESS_RW_INTERLEAVED))
    status = true;

  else if ((access == SND_PCM_ACCESS_RW_NONINTERLEAVED) || (access == SND_PCM_ACCESS_MMAP_NONINTERLEAVED))
    status = false;

  else
    abort();

  return status;

}

void PcmInterface::functionChooser()
{

  if (isInterleaved())
  {
    writeFunctor = [=](void* buf, size_t len) {
      return ( this->writeInterleaved(buf, len) ); };
  }

  else
  {
    writeFunctor = [=](void* buf, size_t len) {
      return this->writeNonInterleaved(buf, len); };
  }

}

snd_pcm_sframes_t PcmInterface::write(void* buffer, snd_pcm_uframes_t size)
{
  snd_pcm_sframes_t frames{};

  if (isInterleaved())
    frames = writeInterleaved(buffer, size);

  else
    frames = writeNonInterleaved(buffer, size);

  return frames;
}

snd_pcm_sframes_t PcmInterface::readInterleaved(void* buffer, snd_pcm_uframes_t size)
{
  int frames = snd_pcm_readi(soundDevice, buffer, size);
  return frames;
}

snd_pcm_sframes_t PcmInterface::readNonInterleaved(void* buffer, snd_pcm_uframes_t size)
{
  int frames = snd_pcm_readn(soundDevice, &buffer, size);
  return frames;
}

snd_pcm_sframes_t PcmInterface::read(void* buffer, snd_pcm_uframes_t size)
{
  snd_pcm_sframes_t frames{};

  if (isInterleaved())
    frames = readInterleaved(buffer, size);

  else
    frames = readNonInterleaved(buffer, size);

  return frames;
}

void PcmInterface::recoverStream(int err, int silent)
{
  int _err = snd_pcm_recover(soundDevice, err, silent);
  if (_err < 0)
    abort();
}

void PcmInterface::stopPresentingFrames()
{
  int err = snd_pcm_drain(soundDevice);
  if (err < 0)
    abort();
}

void PcmInterface::setParam()
{
  int err = snd_pcm_hw_params(soundDevice, hwParams);
  if (err < 0)
    abort(decode_snd_err(err));
}

std::string PcmInterface::getName() const
{
  return std::string{snd_pcm_name(soundDevice)};
}

std::string PcmInterface::getState() const
{
  return std::string{snd_pcm_state_name(snd_pcm_state(soundDevice))};
}

Channels PcmInterface::getChannels() const
{
  unsigned int channels{};
  snd_pcm_hw_params_get_channels(hwParams, &channels);
  return static_cast<Channels>(channels);
}

unsigned int PcmInterface::getRate() const
{
  unsigned int rate{};
  snd_pcm_hw_params_get_rate(hwParams, &rate, 0);
  return rate;
}

snd_pcm_access_t PcmInterface::getAccess()
{
  snd_pcm_access_t access{};
  snd_pcm_hw_params_get_access(hwParams, &access);

  if (!((access >= 0) && (access <= SND_PCM_ACCESS_LAST)))
    abort(decode_snd_err(static_cast<int>(access)));

  return access;
}

void PcmInterface::close()
{
  int err = snd_pcm_close(soundDevice);
  if (err < 0)
    abort();
}

/*
 *
 */
PcmInterface::~PcmInterface()
{

}



} /* namespace Audio */

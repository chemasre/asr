#ifndef _SOUND_HPP_
#define _SOUND_HPP_

#include <common_header.hpp>
#include <common_system_header.hpp>

#define MAX_CHANNELS 8

void enableChannel(int index);
void disableChannel(int index);
void setChannelFrequency(int index, float frequency);
void setChannelVolume(int index, float volume);
void setVolume(float volume);
void startChannelTransition(int index, float frequency, float volume, float time);
int isChannelTransitioning(int index);
void stopChannelTransition(int index);

void enableChannelNoise(int index);
void disableChannelNoise(int index);
void setChannelNoiseFrequency(int index, float frequency);
void setChannelNoiseAmount(int index, float amount);

int reserveChannel();
void releaseChannel(int index);

void initSound();
void updateSound();
void finishSound();

#endif

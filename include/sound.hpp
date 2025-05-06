#ifndef _SOUND_HPP_
#define _SOUND_HPP_

#include <common_header.hpp>
#include <common_system_header.hpp>

#define MAX_CHANNELS 32
#define MAX_SOUNDS 64

#define GENERATOR_TYPE_TONE 0
#define GENERATOR_TYPE_NOISE 1


void muteChannel(int index);
void unmuteChannel(int index);
void setChannelFrequency(int index, float frequency);
void setChannelVolume(int index, float volume);
void setMasterVolume(float volume);

int reserveSound();
void releaseSound(int index);
void playSound(int sound, int channel, float frequency, float volume, float attackDuration, float sustainDuration, float fadeDuration, int loop);
void setSoundFrequency(int index, float frequency);
void setSoundVolume(int index, float volume);
void gotoSoundFrequency(int index, float frequency, float duration);
void gotoSoundVolume(int index, float volume, float duration);
int isSoundGoingVolume(int index);
int isSoundGoingFrequency(int index);
int isSoundPlaying(int index);
int isSoundStopping(int index);
int isSoundPlayingInChannel(int index);
void stopSound(int index);


int reserveChannel(int type);
void releaseChannel(int index);

void initSound();
void updateSound();
void finishSound();

#endif

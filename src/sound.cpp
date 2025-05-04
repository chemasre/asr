#include "sound.hpp"
#include <player.hpp>
#include <portaudio.h>

#define SAMPLE_RATE   (44100)
#define SAMPLES_PER_CALLBACK (64)

PaStream *stream;
PaError soundError;


struct SoundData
{
    float leftPhase;
    float rightPhase;
};

struct Channel
{
    int owned;
    
    int muted;
    float phase;
    float volume;
    float frequency;
    
    int transition;
    float startFrequency;
    float startVolume;
    float targetFrequency;
    float targetVolume;
    float transitionTotalTime;
    float transitionTime;
    
    int noise;    
    float noiseAmount;
    float noiseFrequency;
    float noiseTimer;
    float noisePhase;
    float noiseTargetLevel;
};

float masterVolume;

Channel channels[MAX_CHANNELS];

float soundFrameDeltatime;
static SoundData soundCallbackData;
float soundCallbackTimer;




static int soundCallback( const void *input, void *output, unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                           void *callbackData )
{
    SoundData *soundCallbackData = (SoundData*)callbackData;
    float *outputData = (float*)output;

    soundCallbackTimer += soundFrameDeltatime;
    
    float totalVolume = 0;
    
    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        if(channels[i].owned && !channels[i].muted)
        {
            totalVolume += channels[i].volume;
        }

    }

    for(int i = 0; i < framesPerBuffer; i++ )
    {
        float leftPhase = 0;
        float rightPhase = 0;
        
        for(int j = 0; j < MAX_CHANNELS; j ++)
        {
            if(channels[j].owned && !channels[j].muted)
            {
                if(channels[j].transition)
                {
                    channels[j].transitionTime += soundFrameDeltatime;
                    if(channels[j].transitionTime >= channels[j].transitionTotalTime)
                    {
                        channels[j].transitionTime = channels[j].transitionTotalTime;
                        channels[j].transition = 0;
                    }

                    float f = channels[j].transitionTime / channels[j].transitionTotalTime;
                    
                    
                    float difference = channels[j].targetFrequency - channels[j].startFrequency;
                    channels[j].frequency = channels[j].startFrequency + difference * f;

                    difference = channels[j].targetVolume - channels[j].startVolume;
                    channels[j].volume = channels[j].startVolume + difference * f;
                }

                channels[j].phase += channels[j].frequency * (360.0f) * 1.0f / SAMPLE_RATE; 
                if(channels[j].phase > 360) { channels[j].phase = channels[j].phase - 360.0f;}
                
                float channelPhaseLeft = sin(channels[j].phase * DEG2RAD);
                float channelPhaseRight = cos(channels[j].phase * DEG2RAD);
                
                if(channels[j].noise)
                {                    
                    float noisePeriod = 1.0f / channels[j].noiseFrequency;
                    channels[j].noisePhase +=  (channels[j].noiseTargetLevel - channels[j].noisePhase) * channels[j].noiseTimer / noisePeriod;
                    channels[j].noiseTimer += soundFrameDeltatime;
                    if(channels[j].noiseTimer > noisePeriod)
                    {
                        channels[j].noiseTargetLevel = randomRange(-1000, 1000) / 1000.0f * channels[j].noiseAmount;
                        channels[j].noiseTimer -= noisePeriod;
                    }

                    
                    channelPhaseLeft += channelPhaseLeft + channels[j].noisePhase;
                    channelPhaseRight += channelPhaseRight + channels[j].noisePhase;
                    
                }                
                
				float volumeScale = 1;
				if(totalVolume > 1.0f) { volumeScale = 1 / totalVolume; }
                
				leftPhase += channelPhaseLeft * channels[j].volume * volumeScale;
                rightPhase += channelPhaseRight * channels[j].volume * volumeScale;
            }
            
        }
        
        leftPhase *= masterVolume;
        rightPhase *= masterVolume;
        
        leftPhase = max(-1.0f, min(1.0f, leftPhase));
        rightPhase = max(-1.0f, min(1.0f, rightPhase));
        
        soundCallbackData->leftPhase = leftPhase;
        soundCallbackData->rightPhase = rightPhase;        
        
        *outputData = soundCallbackData->leftPhase;
        outputData ++;
        *outputData = soundCallbackData->rightPhase;
        outputData ++;
        
        soundCallbackTimer += 1.0f / SAMPLE_RATE;
    }
    
    
    return 0;
}



void updateSound()
{

}

int reserveChannel()
{
    int found = -1;
    int index = 0;
    
    while(found < 0 && index < MAX_CHANNELS)
    {
        if(channels[index].owned == 0) { channels[index].owned = 1; found = index; }
        else { index ++; }
        
    }
    
    if(found >= 0) { return found; }
    else { return -1; }
}

void releaseChannel(int index)
{
    channels[index].owned = 0;
}

void muteChannel(int index)
{
    channels[index].muted = 1;
    
}

void unmuteChannel(int index)
{
    channels[index].muted = 0;
}

void setChannelFrequency(int index, float frequency)
{
    channels[index].frequency = frequency;
}

void setChannelVolume(int index, float volume)
{
    channels[index].volume = volume;
}

void setVolume(float volume)
{
    masterVolume = volume;
}

void startChannelTransition(int index, float frequency, float volume, float time)
{
    channels[index].transition = 1;
    channels[index].startFrequency = channels[index].frequency;
    channels[index].startVolume = channels[index].volume;
    if(frequency >= 0) { channels[index].targetFrequency = frequency; }
    else { channels[index].targetFrequency = channels[index].frequency; }
    if(volume >= 0) { channels[index].targetVolume = volume; }
    else { channels[index].targetVolume = channels[index].volume; }
        
    channels[index].transitionTotalTime = time;
    channels[index].transitionTime = 0;
    
}

void stopChannelTransition(int index)
{
    channels[index].transition = 0;
}

int isChannelTransitioning(int index)
{
    return channels[index].transition;
}

void enableChannelNoise(int index)
{
    channels[index].noise = 1;
    channels[index].noiseTimer = 0;
}

void disableChannelNoise(int index)
{
    channels[index].noise = 0;
}

void setChannelNoiseFrequency(int index, float frequency)
{
    channels[index].noiseFrequency = frequency;
}

void setChannelNoiseAmount(int index, float amount)
{
    channels[index].noiseAmount = amount;
}

void initSound()
{
    soundFrameDeltatime = 1.0f / SAMPLE_RATE;

    soundCallbackData.leftPhase = 0.0f;
    soundCallbackData.rightPhase = 0.0f;

    for(int i = 0; i < MAX_CHANNELS; i++)
    {        
        channels[i].owned = 0;
        channels[i].muted = 0;
        channels[i].phase = 0;
        channels[i].volume = 0;
        channels[i].frequency = 0; 
        channels[i].transition = 0;
        channels[i].noise = 0;
        channels[i].noiseAmount = 0.5f;
        channels[i].noiseFrequency = 1000.0f;
        channels[i].noisePhase = 0;
        
    }
    
    masterVolume = 1;
    
    soundCallbackTimer = 0;
    
    soundError = Pa_Initialize();   
    
    ASSERT(soundError == paNoError,"Initialize audio error")

    soundError = Pa_OpenDefaultStream( &stream, 0, 2, paFloat32, SAMPLE_RATE, SAMPLES_PER_CALLBACK, soundCallback, &soundCallbackData );
                                
    ASSERT(soundError == paNoError,"Open audio stream error")
    
    soundError = Pa_StartStream( stream );
    
    ASSERT(soundError == paNoError,"Cannot start audio stream")

}



void finishSound()
{
    soundError = Pa_StopStream( stream );
    ASSERT(soundError == paNoError,"Error stopping audio stream")

    soundError = Pa_CloseStream( stream );
    
    ASSERT(soundError == paNoError,"Error closeing audio stream")
    
    Pa_Terminate();    
    
}
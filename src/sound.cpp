#include "sound.hpp"
#include <player.hpp>
#include <portaudio.h>

#define SAMPLE_RATE   (44100)
#define SAMPLE_DELTA_TIME (1.0f / SAMPLE_RATE)
#define SAMPLES_PER_CALLBACK (64)

PaStream *stream;
PaError soundError;


struct SoundData
{
    float leftOutput;
    float rightOutput;
};

struct Channel
{
    int owned;
    
    int muted;
    float phase;
    float volume;
    float frequency;
    float output;   

};

struct ChannelTransition
{
    int active;
    float startFrequency;
    float startVolume;
    float targetFrequency;
    float targetVolume;
    float totalTime;
    float time;
};

struct ChannelGenerator
{
    int type;    
    
    // TONE
    
    float toneAngle;

    // NOISE
    
    float noiseChangePhaseTimer;
    float noisePhase;
    float noiseTargetPhase;
};

float masterVolume;

Channel channels[MAX_CHANNELS];
ChannelGenerator channelGenerators[MAX_CHANNELS];
ChannelTransition channelTransitions[MAX_CHANNELS];

static SoundData soundCallbackData;
float soundCallbackTimer;




static int soundCallback( const void *input, void *output, unsigned long samplesPerCallback,
                          const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                           void *callbackData )
{
    SoundData *soundCallbackData = (SoundData*)callbackData;
    float *outputData = (float*)output;
    
    float totalVolume = 0;
    float meanVolume = 0;
    int activeChannelsCount = 0;
    
    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        if(channels[i].owned && !channels[i].muted)
        {
            activeChannelsCount ++;
            totalVolume += channels[i].volume;
        }

    }
    
    if(activeChannelsCount > 0) { meanVolume = totalVolume / activeChannelsCount; }

    for(int s = 0; s < samplesPerCallback; s++ )
    {
        for(int c = 0; c < MAX_CHANNELS; c ++)
        {
            if(channels[c].owned && !channels[c].muted)
            {
                if(channelTransitions[c].active)
                {
                    channelTransitions[c].time += SAMPLE_DELTA_TIME;
                    if(channelTransitions[c].time >= channelTransitions[c].totalTime)
                    {
                        channelTransitions[c].time = channelTransitions[c].totalTime;
                        channelTransitions[c].active = 0;
                    }

                    float f = channelTransitions[c].time / channelTransitions[c].totalTime;
                    
                    
                    float difference = channelTransitions[c].targetFrequency - channelTransitions[c].startFrequency;
                    channels[c].frequency = channelTransitions[c].startFrequency + difference * f;

                    difference = channelTransitions[c].targetVolume - channelTransitions[c].startVolume;
                    channels[c].volume = channelTransitions[c].startVolume + difference * f;
                }
                
                if(channelGenerators[c].type == GENERATOR_TYPE_TONE)
                {
                    channelGenerators[c].toneAngle += channels[c].frequency * (360.0f) * 1.0f / SAMPLE_RATE; 
                    if(channelGenerators[c].toneAngle > 360) { channelGenerators[c].toneAngle = channelGenerators[c].toneAngle - 360.0f;}
                    channels[c].output = sin(channelGenerators[c].toneAngle * DEG2RAD);
                }
                else
                {
                    float noisePeriod = 1.0f / channels[c].frequency;
                    channelGenerators[c].noisePhase +=  (channelGenerators[c].noiseTargetPhase - channelGenerators[c].noisePhase) * channelGenerators[c].noiseChangePhaseTimer / noisePeriod;
                    channelGenerators[c].noiseChangePhaseTimer += SAMPLE_DELTA_TIME;
                    if(channelGenerators[c].noiseChangePhaseTimer > noisePeriod)
                    {
                        channelGenerators[c].noiseTargetPhase = randomRange(-1000, 1000) / 1000.0f;
                        channelGenerators[c].noiseChangePhaseTimer -= noisePeriod;
                    }
                    
                    channels[c].output = channelGenerators[c].noisePhase;
                }
                
                
            }
            
        }
        
        float leftOutput = 0;
        float rightOutput = 0;

        if(totalVolume > 0)
        {
            for(int j = 0; j < MAX_CHANNELS; j++)
            {
                leftOutput += channels[j].output * channels[j].volume / totalVolume * meanVolume;
                rightOutput += channels[j].output * channels[j].volume / totalVolume * meanVolume;
            }
        }
        
        leftOutput *= masterVolume;
        rightOutput *= masterVolume;
        
        leftOutput = max(-1.0f, min(1.0f, leftOutput));
        rightOutput = max(-1.0f, min(1.0f, rightOutput));
        
        soundCallbackData->leftOutput = leftOutput;
        soundCallbackData->rightOutput = rightOutput;        
        
        *outputData = soundCallbackData->leftOutput;
        outputData ++;
        *outputData = soundCallbackData->rightOutput;
        outputData ++;

    }
    
    
    return 0;
}



void updateSound()
{

}

int reserveChannel(int generatorType)
{
    int found = -1;
    int index = 0;
    
    while(found < 0 && index < MAX_CHANNELS)
    {
        if(channels[index].owned == 0)
        {
            channels[index].owned = 1;
            channelGenerators[index].type = generatorType;
            if(generatorType == GENERATOR_TYPE_TONE) { channelGenerators[index].toneAngle = 0; }
            channelTransitions[index].active = 0;    
            found = index;
        }
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
    channelTransitions[index].active = 1;
    channelTransitions[index].startFrequency = channels[index].frequency;
    channelTransitions[index].startVolume = channels[index].volume;
    if(frequency >= 0) { channelTransitions[index].targetFrequency = frequency; }
    else { channelTransitions[index].targetFrequency = channels[index].frequency; }
    if(volume >= 0) { channelTransitions[index].targetVolume = volume; }
    else { channelTransitions[index].targetVolume = channels[index].volume; }
        
    channelTransitions[index].totalTime = time;
    channelTransitions[index].time = 0;
    
}

void stopChannelTransition(int index)
{
    channelTransitions[index].active = 0;
}

int isChannelTransitioning(int index)
{
    return channelTransitions[index].active;
}

void initSound()
{
    soundCallbackData.leftOutput = 0.0f;
    soundCallbackData.rightOutput = 0.0f;

    for(int i = 0; i < MAX_CHANNELS; i++)
    {        
        channels[i].owned = 0;
        channels[i].muted = 0;

        channels[i].volume = 0;
        channels[i].frequency = 0;         

        channels[i].output = 0;
  
        channelGenerators[i].type = GENERATOR_TYPE_TONE;
        channelGenerators[i].toneAngle = 0;
        channelGenerators[i].noisePhase = 0;

        channelTransitions[i].active = 0;    
    }
    
    
    
    masterVolume = 1;
    
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
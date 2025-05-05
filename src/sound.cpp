#include "sound.hpp"
#include <player.hpp>
#include <portaudio.h>

#define SAMPLE_RATE   (44100)
#define SAMPLE_DELTA_TIME (1.0f / SAMPLE_RATE)
#define SAMPLES_PER_CALLBACK (64)

#define SOUND_STATE_STOPPED 0
#define SOUND_STATE_PLAYING 1
#define SOUND_STATE_STOPPING 2


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

struct ChannelSound
{
    int state;
    float frequency;
    float volume;
    float time;
    float attackDuration;
    float sustainDuration;
    float fadeDuration;
    int loop;

    int gotoFrequency;
    float gotoFrequencyTimer;
    float gotoFrequencyStart;
    float gotoFrequencyEnd;
    float gotoFrequencyDuration;
    
    int gotoVolume;
    float gotoVolumeTimer;
    float gotoVolumeStart;
    float gotoVolumeEnd;
    float gotoVolumeDuration;
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
ChannelSound channelSounds[MAX_CHANNELS];

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
        if(channels[i].owned && !channels[i].muted && (channelSounds[i].state == SOUND_STATE_PLAYING || channelSounds[i].state == SOUND_STATE_STOPPING))
        {
            activeChannelsCount ++;
            totalVolume += channels[i].volume;
        }

    }
    
    if(activeChannelsCount > 0) { meanVolume = totalVolume / activeChannelsCount; }
    
    for(int s = 0; s < samplesPerCallback; s++ )
    {
        for(int c = 0; c < MAX_CHANNELS; c++)
        {
            if(channelSounds[c].state == SOUND_STATE_PLAYING)
            {
                if(channelSounds[c].gotoFrequency)
                {
                    channelSounds[c].gotoFrequencyTimer += SAMPLE_DELTA_TIME;
                    
                    if(channelSounds[c].gotoFrequencyTimer >= channelSounds[c].gotoFrequencyDuration)
                    {
                        channelSounds[c].frequency = channelSounds[c].gotoFrequencyEnd;
                        channelSounds[c].gotoFrequency = 0;
                    }
                    else
                    {
                        channelSounds[c].frequency = channelSounds[c].gotoFrequencyStart + (channelSounds[c].gotoFrequencyEnd - channelSounds[c].gotoFrequencyStart) * channelSounds[c].gotoFrequencyTimer / channelSounds[c].gotoFrequencyDuration;
                    }
                }
                
                if(channelSounds[c].gotoVolume)
                {
                    channelSounds[c].gotoVolumeTimer += SAMPLE_DELTA_TIME;
                    
                    if(channelSounds[c].gotoVolumeTimer >= channelSounds[c].gotoVolumeDuration)
                    {
                        channelSounds[c].volume = channelSounds[c].gotoVolumeEnd;
                        channelSounds[c].gotoVolume = 0;                        
                    }
                    else
                    {
                        channelSounds[c].frequency = channelSounds[c].gotoVolumeStart + (channelSounds[c].gotoVolumeEnd - channelSounds[c].gotoVolumeStart) * channelSounds[c].gotoVolumeTimer / channelSounds[c].gotoVolumeDuration;
                    }
                    
                }                
                
                
                channelSounds[c].time += SAMPLE_DELTA_TIME;
                
                if(channelSounds[c].time <= channelSounds[c].attackDuration)
                {
                    channels[c].frequency = channelSounds[c].frequency;
                    
                    if(channelSounds[c].attackDuration > 0)
                    {
                        channels[c].volume = channelSounds[c].time / channelSounds[c].attackDuration * channelSounds[c].volume;
                    }
                    else
                    {
                        channels[c].volume = 1;
                    }
                }
                else if(channelSounds[c].time <= channelSounds[c].attackDuration + channelSounds[c].sustainDuration)
                {
                    channels[c].frequency = channelSounds[c].frequency;
                    channels[c].volume = channelSounds[c].volume;
                }
                else if(channelSounds[c].loop)
                {
                    channelSounds[c].time = channelSounds[c].attackDuration;
                    channels[c].frequency = channelSounds[c].frequency;
                    channels[c].volume = channelSounds[c].volume;
                }                
                else if(channelSounds[c].time <= channelSounds[c].attackDuration + channelSounds[c].sustainDuration + channelSounds[c].fadeDuration)
                {
                    float t = channelSounds[c].time - (channelSounds[c].attackDuration + channelSounds[c].sustainDuration);
                    channels[c].frequency = channelSounds[c].frequency;
                    
                    if(channelSounds[c].fadeDuration > 0)
                    {
                        channels[c].volume = channelSounds[c].volume * (1 - t / channelSounds[c].fadeDuration);
                    }
                    else
                    {
                        channels[c].volume = 0;
                    }
                }
                else
                {
                    channels[c].frequency = channelSounds[c].frequency;
                    channels[c].volume = 0;
                    channelSounds[c].state = SOUND_STATE_STOPPED;
                }
            }
            else if(channelSounds[c].state == SOUND_STATE_STOPPING)
            {
                channelSounds[c].time += SAMPLE_DELTA_TIME;

                if(channelSounds[c].time <= channelSounds[c].attackDuration + channelSounds[c].sustainDuration + channelSounds[c].fadeDuration)
                {
                    float t = channelSounds[c].time - (channelSounds[c].attackDuration + channelSounds[c].sustainDuration);
                    channels[c].frequency = channelSounds[c].frequency;
                    
                    if(channelSounds[c].fadeDuration > 0)
                    {
                        channels[c].volume = channelSounds[c].volume * (1 - t / channelSounds[c].fadeDuration);
                    }
                    else
                    {
                        channels[c].volume = 0;
                    }                
                }
                else
                {
                    channels[c].frequency = channelSounds[c].frequency;
                    channels[c].volume = 0;
                    channelSounds[c].state = SOUND_STATE_STOPPED;
                }                
            }
            
        }

        for(int c = 0; c < MAX_CHANNELS; c ++)
        {
            if(channels[c].owned && !channels[c].muted && (channelSounds[c].state == SOUND_STATE_PLAYING || channelSounds[c].state == SOUND_STATE_STOPPING))
            {
                
                if(channelGenerators[c].type == GENERATOR_TYPE_TONE)
                {
                    channelGenerators[c].toneAngle += channels[c].frequency * (360.0f) * SAMPLE_DELTA_TIME; 
                    if(channelGenerators[c].toneAngle > 360) { channelGenerators[c].toneAngle = channelGenerators[c].toneAngle - 360.0f;}
                    channels[c].output = sin(channelGenerators[c].toneAngle * DEG2RAD);
                }
                else
                {
                    ASSERT(channels[c].frequency > 0, "Frequency is zero");
                    float noisePeriod = 1.0f / channels[c].frequency;
                    ASSERT(noisePeriod > 0, "Noise period is zero");
                    
                    
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
            channels[index].volume = 0;
            channels[index].frequency = 1; 
            channels[index].output = 0;
            channelGenerators[index].type = generatorType;
            if(generatorType == GENERATOR_TYPE_TONE)
            {
                channelGenerators[index].toneAngle = 0;
            }
            else
            {
                channelGenerators[index].noiseTargetPhase = 0;
                channelGenerators[index].noisePhase = 0;
                channelGenerators[index].noiseChangePhaseTimer = 0;
                
            }
            channelSounds[index].state = SOUND_STATE_STOPPED;    
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

void playSound(int index, float frequency, float volume, float attackDuration, float sustainDuration, float fadeDuration, int loop)
{
    channelSounds[index].state = SOUND_STATE_PLAYING;
    channelSounds[index].volume = volume;
    channelSounds[index].frequency = frequency;
    channelSounds[index].attackDuration = attackDuration;
    channelSounds[index].sustainDuration = sustainDuration;
    channelSounds[index].fadeDuration = fadeDuration;
    channelSounds[index].loop = loop;
    channelSounds[index].time = 0;
    channelSounds[index].gotoFrequency = 0;
    channelSounds[index].gotoVolume = 0;
    
}

void stopSound(int index)
{
    if(channelSounds[index].state == SOUND_STATE_PLAYING)
    {
        channelSounds[index].gotoFrequency = 0;
        channelSounds[index].gotoVolume = 0;

        channelSounds[index].time = channelSounds[index].attackDuration + channelSounds[index].sustainDuration;
        channelSounds[index].volume = channels[index].volume;
        channelSounds[index].state = SOUND_STATE_STOPPING;
        
    }
}

int isSoundPlaying(int index)
{
    return channelSounds[index].state == SOUND_STATE_PLAYING;
}

void gotoSoundFrequency(int index, float frequency, float duration)
{
    if(channelSounds[index].state == SOUND_STATE_PLAYING)
    {
        channelSounds[index].gotoFrequency = 1;
        channelSounds[index].gotoFrequencyTimer = 0;
        channelSounds[index].gotoFrequencyStart = channelSounds[index].frequency;
        channelSounds[index].gotoFrequencyEnd = frequency;
        channelSounds[index].gotoFrequencyDuration = duration;
        
    }
}

void gotoSoundVolume(int index, float volume, float duration)
{
    if(channelSounds[index].state == SOUND_STATE_PLAYING)
    {
        channelSounds[index].gotoVolume = 1;
        channelSounds[index].gotoVolumeTimer = 0;
        channelSounds[index].gotoVolumeStart = channelSounds[index].volume;
        channelSounds[index].gotoVolumeEnd = volume;
        channelSounds[index].gotoVolumeDuration = duration;
        
    }
    
}

int isSoundGoingVolume(int index)
{
    return channelSounds[index].state == SOUND_STATE_PLAYING && channelSounds[index].gotoVolume;
}

int isSoundGoingFrequency(int index)
{
    return channelSounds[index].state == SOUND_STATE_PLAYING && channelSounds[index].gotoFrequency;
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
        channels[i].frequency = 1;         

        channels[i].output = 0;
  
        channelGenerators[i].type = GENERATOR_TYPE_TONE;
        
        channelGenerators[i].toneAngle = 0;
        
        channelGenerators[i].noiseChangePhaseTimer = 0;
        channelGenerators[i].noisePhase = 0;
        channelGenerators[i].noiseTargetPhase = 0;

        channelSounds[i].state = SOUND_STATE_STOPPED;    

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
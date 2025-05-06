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
    int reserved;
    
    int muted;
    int playingSound;
    float volume;
    float frequency;
    float output;   

};

struct Sound
{
    int reserved;
    
    int channel;
    int playState;
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

struct Generator
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
Generator generators[MAX_CHANNELS];
Sound sounds[MAX_SOUNDS];

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
    int playingChannelsCount = 0;
    
    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        if(channels[i].reserved && !channels[i].muted && channels[i].playingSound)
        {
            playingChannelsCount ++;
            totalVolume += channels[i].volume;
        }

    }
    
    if(playingChannelsCount > 0) { meanVolume = totalVolume / playingChannelsCount; }
    
    for(int k = 0; k < samplesPerCallback; k++ )
    {
        for(int s = 0; s < MAX_SOUNDS; s++)
        {
            if(sounds[s].reserved)
            {
                int c = sounds[s].channel;
                
                if(sounds[s].playState == SOUND_STATE_PLAYING)
                {
                    // Update sound gotos
                    
                    if(sounds[s].gotoFrequency)
                    {
                        sounds[s].gotoFrequencyTimer += SAMPLE_DELTA_TIME;
                        
                        if(sounds[s].gotoFrequencyTimer >= sounds[s].gotoFrequencyDuration)
                        {
                            sounds[s].frequency = sounds[s].gotoFrequencyEnd;
                            sounds[s].gotoFrequency = 0;
                        }
                        else
                        {
                            sounds[s].frequency = sounds[s].gotoFrequencyStart + (sounds[s].gotoFrequencyEnd - sounds[s].gotoFrequencyStart) * sounds[s].gotoFrequencyTimer / sounds[s].gotoFrequencyDuration;
                        }
                    }
                    
                    if(sounds[s].gotoVolume)
                    {
                        sounds[s].gotoVolumeTimer += SAMPLE_DELTA_TIME;
                        
                        if(sounds[s].gotoVolumeTimer >= sounds[s].gotoVolumeDuration)
                        {
                            sounds[s].volume = sounds[s].gotoVolumeEnd;
                            sounds[s].gotoVolume = 0;                        
                        }
                        else
                        {
                            sounds[s].volume = sounds[s].gotoVolumeStart + (sounds[s].gotoVolumeEnd - sounds[s].gotoVolumeStart) * sounds[s].gotoVolumeTimer / sounds[s].gotoVolumeDuration;
                        }
                        
                    }     

                    // update channel frequency and volume
                    
                    sounds[s].time += SAMPLE_DELTA_TIME;
                    
                    if(sounds[s].time <= sounds[s].attackDuration)
                    {
                        channels[c].frequency = sounds[s].frequency;
                        
                        if(sounds[s].attackDuration > 0)
                        {
                            channels[c].volume = sounds[s].time / sounds[s].attackDuration * sounds[s].volume;
                        }
                        else
                        {
                            channels[c].volume = 1;
                        }
                    }
                    else if(sounds[s].time <= sounds[s].attackDuration + sounds[s].sustainDuration)
                    {
                        channels[c].frequency = sounds[s].frequency;
                        channels[c].volume = sounds[s].volume;
                    }
                    else if(sounds[s].loop)
                    {
                        sounds[s].time = sounds[s].attackDuration;
                        channels[c].frequency = sounds[s].frequency;
                        channels[c].volume = sounds[s].volume;
                    }                
                    else if(sounds[s].time <= sounds[s].attackDuration + sounds[s].sustainDuration + sounds[s].fadeDuration)
                    {
                        float t = sounds[s].time - (sounds[s].attackDuration + sounds[s].sustainDuration);
                        channels[c].frequency = sounds[s].frequency;
                        
                        if(sounds[s].fadeDuration > 0)
                        {
                            channels[c].volume = sounds[s].volume * (1 - t / sounds[s].fadeDuration);
                        }
                        else
                        {
                            channels[c].volume = 0;
                        }
                    }
                    else
                    {
                        channels[c].frequency = sounds[s].frequency;
                        channels[c].volume = 0;
                        sounds[s].playState = SOUND_STATE_STOPPED;
                        channels[c].playingSound = 0;
                    }
                }
                else if(sounds[s].playState == SOUND_STATE_STOPPING)
                {
                    sounds[s].time += SAMPLE_DELTA_TIME;

                    if(sounds[s].time <= sounds[s].attackDuration + sounds[s].sustainDuration + sounds[s].fadeDuration)
                    {
                        float t = sounds[s].time - (sounds[s].attackDuration + sounds[s].sustainDuration);
                        channels[c].frequency = sounds[s].frequency;
                        
                        if(sounds[s].fadeDuration > 0)
                        {
                            channels[c].volume = sounds[s].volume * (1 - t / sounds[s].fadeDuration);
                        }
                        else
                        {
                            channels[c].volume = 0;
                        }                
                    }
                    else
                    {
                        channels[c].frequency = sounds[s].frequency;
                        channels[c].volume = 0;
                        sounds[s].playState = SOUND_STATE_STOPPED;
                        channels[c].playingSound = 0;
                    }                
                }

            }
            
            
        }

        for(int c = 0; c < MAX_CHANNELS; c ++)
        {
            if(channels[c].reserved && !channels[c].muted && channels[c].playingSound)
            {
                
                if(generators[c].type == GENERATOR_TYPE_TONE)
                {
                    generators[c].toneAngle += channels[c].frequency * (360.0f) * SAMPLE_DELTA_TIME; 
                    if(generators[c].toneAngle > 360) { generators[c].toneAngle = generators[c].toneAngle - 360.0f;}
                    channels[c].output = sin(generators[c].toneAngle * DEG2RAD);
                }
                else
                {
                    ASSERT(channels[c].frequency > 0, "Frequency is zero");
                    float noisePeriod = 1.0f / channels[c].frequency;
                    ASSERT(noisePeriod > 0, "Noise period is zero");
                    
                    
                    generators[c].noisePhase +=  (generators[c].noiseTargetPhase - generators[c].noisePhase) * generators[c].noiseChangePhaseTimer / noisePeriod;
                    generators[c].noiseChangePhaseTimer += SAMPLE_DELTA_TIME;
                    if(generators[c].noiseChangePhaseTimer > noisePeriod)
                    {
                        generators[c].noiseTargetPhase = randomRange(-1000, 1000) / 1000.0f;
                        generators[c].noiseChangePhaseTimer -= noisePeriod;
                    }
                    
                    channels[c].output = generators[c].noisePhase;                    
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
        if(channels[index].reserved == 0)
        {
            channels[index].reserved = 1;
            channels[index].muted = 0;
            channels[index].playingSound = 0;
            
            channels[index].volume = 0;
            channels[index].frequency = 1; 
            channels[index].output = 0;
            
            generators[index].type = generatorType;
            if(generatorType == GENERATOR_TYPE_TONE)
            {
                generators[index].toneAngle = 0;
            }
            else
            {
                generators[index].noiseTargetPhase = 0;
                generators[index].noisePhase = 0;
                generators[index].noiseChangePhaseTimer = 0;
                
            }
            sounds[index].playState = SOUND_STATE_STOPPED;    
            found = index;
        }
        else { index ++; }
        
    }
    
    if(found >= 0) { return found; }
    else { return -1; }
}

void releaseChannel(int index)
{
    channels[index].reserved = 0;
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

void setMasterVolume(float volume)
{
    masterVolume = volume;
}

int reserveSound()
{
    int foundIndex = -1;
    int index = 0;
    
    while(index < MAX_SOUNDS && foundIndex < 0)
    {
        if(!sounds[index].reserved)
        {
            sounds[index].reserved = 1;
            foundIndex = index;

        }
        else
        {
            index ++;
        }
    }
    
    return foundIndex;
    
}

void releaseSound(int index)
{
    ASSERT(index >= 0 && index < MAX_SOUNDS, "releaseSound: Sound is out of range") 
    
    sounds[index].reserved = 0;
}

void playSound(int sound, int channel, float frequency, float volume, float attackDuration, float sustainDuration, float fadeDuration, int loop)
{
    ASSERT(sound >= 0 && sound < MAX_SOUNDS, "playSound: Sound is out of range") 
    ASSERT(sounds[sound].reserved, "playSound: Sound is not reserved") 
    ASSERT(channel >= 0 && channel < MAX_CHANNELS, "playSound: Channel is out of range") 
    ASSERT(channels[channel].reserved, "playSound: Channel is not reserved") 
    
    sounds[sound].playState = SOUND_STATE_PLAYING;
    sounds[sound].channel = channel;
    channels[channel].playingSound = 1;
    
    sounds[sound].volume = volume;
    sounds[sound].frequency = frequency;
    sounds[sound].attackDuration = attackDuration;
    sounds[sound].sustainDuration = sustainDuration;
    sounds[sound].fadeDuration = fadeDuration;
    sounds[sound].loop = loop;
    sounds[sound].time = 0;
    sounds[sound].gotoFrequency = 0;
    sounds[sound].gotoVolume = 0;   

    
}

void stopSound(int index)
{    
    ASSERT(index >= 0 && index < MAX_SOUNDS, "stopSound: Sound index out of range") 
    ASSERT(sounds[index].reserved, "stopSound: Sound is not reserved")

    if(sounds[index].playState == SOUND_STATE_PLAYING)
    {
        sounds[index].gotoFrequency = 0;
        sounds[index].gotoVolume = 0;

        sounds[index].time = sounds[index].attackDuration + sounds[index].sustainDuration;
        sounds[index].volume = channels[index].volume;
        sounds[index].playState = SOUND_STATE_STOPPING;
        
    }
}

int isSoundPlaying(int index)
{ 
    ASSERT(index >= 0 && index < MAX_SOUNDS, "isSoundPlaying: Sound index out of range") 
    ASSERT(sounds[index].reserved, "isSoundPlaying: Sound is not reserved")
  
    return (sounds[index].playState == SOUND_STATE_PLAYING || sounds[index].playState == SOUND_STATE_STOPPING);
}

int isSoundStopping(int index)
{ 
    ASSERT(index >= 0 && index < MAX_SOUNDS, "isSoundPlaying: Sound index out of range") 
    ASSERT(sounds[index].reserved, "isSoundPlaying: Sound is not reserved")
  
    return (sounds[index].playState == SOUND_STATE_STOPPING);
}


int isSoundPlayingInChannel(int index)
{
    ASSERT(index >= 0 && index < MAX_CHANNELS, "isSoundPlayingInChannel: Channel index out of range") 
    ASSERT(channels[index].reserved, "isSoundPlayingInChannel: Channel is not reserved")

    return channels[index].playingSound;
}

void gotoSoundFrequency(int index, float frequency, float duration)
{
    ASSERT(index >= 0 && index < MAX_SOUNDS, "gotoSoundFrequency: Sound index out of range") 
    ASSERT(sounds[index].reserved, "gotoSoundFrequency: Sound is not reserved")
    
    if(sounds[index].playState == SOUND_STATE_PLAYING)
    {
        sounds[index].gotoFrequency = 1;
        sounds[index].gotoFrequencyTimer = 0;
        sounds[index].gotoFrequencyStart = sounds[index].frequency;
        sounds[index].gotoFrequencyEnd = frequency;
        sounds[index].gotoFrequencyDuration = duration;
        
    }
}

void gotoSoundVolume(int index, float volume, float duration)
{
    ASSERT(index >= 0 && index < MAX_SOUNDS, "gotoSoundVolume: Sound index out of range") 
    ASSERT(sounds[index].reserved, "gotoSoundVolume: Sound is not reserved")

    if(sounds[index].playState == SOUND_STATE_PLAYING)
    {
        sounds[index].gotoVolume = 1;
        sounds[index].gotoVolumeTimer = 0;
        sounds[index].gotoVolumeStart = sounds[index].volume;
        sounds[index].gotoVolumeEnd = volume;
        sounds[index].gotoVolumeDuration = duration;
        
    }
    
}

int isSoundGoingVolume(int index)
{
    ASSERT(index >= 0 && index < MAX_SOUNDS, "isSoundGoingVolume: Sound index out of range") 
    ASSERT(sounds[index].reserved, "isSoundGoingVolume: Sound is not reserved")
    
    return sounds[index].playState == SOUND_STATE_PLAYING && sounds[index].gotoVolume;
}

int isSoundGoingFrequency(int index)
{
    ASSERT(index >= 0 && index < MAX_SOUNDS, "isSoundGoingFrequency: Sound index out of range") 
    ASSERT(sounds[index].reserved, "isSoundGoingFrequency: Sound is not reserved")

    return sounds[index].playState == SOUND_STATE_PLAYING && sounds[index].gotoFrequency;
}

void initSound()
{
    soundCallbackData.leftOutput = 0.0f;
    soundCallbackData.rightOutput = 0.0f;
    
    for(int i = 0; i < MAX_SOUNDS; i++)
    {
        sounds[i].reserved = 0;
        sounds[i].playState = SOUND_STATE_STOPPED;    
    }

    for(int i = 0; i < MAX_CHANNELS; i++)
    {        
        channels[i].reserved = 0;
        channels[i].muted = 0;
        channels[i].playingSound = 0;

        channels[i].volume = 0;
        channels[i].frequency = 1;         

        channels[i].output = 0;
  
        generators[i].type = GENERATOR_TYPE_TONE;
        
        generators[i].toneAngle = 0;
        
        generators[i].noiseChangePhaseTimer = 0;
        generators[i].noisePhase = 0;
        generators[i].noiseTargetPhase = 0;


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
#include <music.hpp>


#define MUSIC_FREQUENCY_HIGH_MIN 300
#define MUSIC_FREQUENCY_HIGH_MAX 150
#define MUSIC_FREQUENCY_LOW_MIN 40
#define MUSIC_FREQUENCY_LOW_MAX 150
#define MUSIC_IN_MIN 1
#define MUSIC_IN_MAX 3
#define MUSIC_OUT_MIN 6
#define MUSIC_OUT_MAX 10
#define MUSIC_WAIT_MIN 3
#define MUSIC_WAIT_MAX 12

float musicVolume;
float musicTimer;

int musicState;
float musicSilenceWait;

int musicChannelHigh;
int musicChannelLow;

void initMusic()
{
    musicChannelHigh = reserveChannel();
    musicChannelLow = reserveChannel();
    musicVolume = 0.1f;
    musicTimer = 0;

    enableChannelNoise(musicChannelLow);
    enableChannelNoise(musicChannelHigh);

    setChannelFrequency(musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX));
    setChannelFrequency(musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX));
    setChannelNoiseFrequency(musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX));
    setChannelNoiseFrequency(musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX));
    setChannelNoiseAmount(musicChannelLow, randomRange(0, 100) / 100.0f);
    setChannelNoiseAmount(musicChannelHigh, randomRange(0, 100) / 100.0f);
    setChannelVolume(musicChannelHigh, 0);
    setChannelVolume(musicChannelLow, 0);
    
    float duration = randomRange(MUSIC_IN_MIN, MUSIC_IN_MAX);
    startChannelTransition(musicChannelHigh, -1, musicVolume, duration);
    startChannelTransition(musicChannelLow, -1, musicVolume, duration);
    musicState = 0;
                          
    musicSilenceWait = 0;
    
}


void updateMusic()
{
    if(musicState == 2)
    {
        musicSilenceWait -= 1.0f / SCREEN_FPS;
        if(musicSilenceWait <= 0)
        {
            setChannelFrequency(musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX));
            setChannelFrequency(musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX));
            setChannelNoiseFrequency(musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX));
            setChannelNoiseFrequency(musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX));
            setChannelNoiseAmount(musicChannelLow, randomRange(0, 100) / 100.0f);
            setChannelNoiseAmount(musicChannelHigh, randomRange(0, 100) / 100.0f);
            setChannelVolume(musicChannelHigh, 0);
            setChannelVolume(musicChannelLow, 0);
            
            float duration = randomRange(MUSIC_IN_MIN, MUSIC_IN_MAX);
            startChannelTransition(musicChannelHigh, -1, musicVolume, duration);
            startChannelTransition(musicChannelLow, -1, musicVolume, duration);
            musicState = 0;
        }
    }
    
    if(!isChannelTransitioning(musicChannelHigh) && !isChannelTransitioning(musicChannelLow))
    {
        if(musicState == 0)
        {
            float duration = randomRange(MUSIC_OUT_MIN, MUSIC_OUT_MAX);
            startChannelTransition(musicChannelHigh, -1, 0, duration);
            startChannelTransition(musicChannelLow, -1, 0, duration);
            musicState = 1;
        }
        else if(musicState == 1)
        {
            musicSilenceWait = randomRange(MUSIC_WAIT_MIN, MUSIC_WAIT_MAX);
            musicState = 2;
        }
        
    }
    
}

void finishMusic()
{
    releaseChannel(musicChannelHigh);
    releaseChannel(musicChannelLow);
}
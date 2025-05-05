#include <music.hpp>


#define MUSIC_FREQUENCY_HIGH_MIN 150
#define MUSIC_FREQUENCY_HIGH_MAX 300
#define MUSIC_FREQUENCY_LOW_MAX 150
#define MUSIC_FREQUENCY_LOW_MIN 40
#define MUSIC_IN_MIN 1
#define MUSIC_IN_MAX 3
#define MUSIC_SUSTAIN_MIN 3
#define MUSIC_SUSTAIN_MAX 6
#define MUSIC_OUT_MIN 3
#define MUSIC_OUT_MAX 4
#define MUSIC_WAIT_MIN 3
#define MUSIC_WAIT_MAX 12

float musicVolume;
float musicTimer;

int musicState;
float musicSilenceWait;

int musicChannelHigh;
int musicChannelNoiseHigh;

int musicChannelLow;
int musicChannelNoiseLow;

void initMusic()
{
    musicChannelHigh = reserveChannel(GENERATOR_TYPE_TONE);
    musicChannelNoiseHigh = reserveChannel(GENERATOR_TYPE_NOISE);
    musicChannelLow = reserveChannel(GENERATOR_TYPE_TONE);
    musicChannelNoiseLow = reserveChannel(GENERATOR_TYPE_NOISE);
    
    musicVolume = 0.3f;
    musicTimer = 0;


    float inDuration = randomRange(MUSIC_IN_MIN, MUSIC_IN_MAX);
    float sustainDuration = randomRange(MUSIC_SUSTAIN_MIN, MUSIC_SUSTAIN_MAX);
    float outDuration = randomRange(MUSIC_OUT_MIN, MUSIC_OUT_MAX);
    
    playSound(musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    playSound(musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    playSound(musicChannelNoiseHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    playSound(musicChannelNoiseLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    musicState = 0;
                          
    musicSilenceWait = 0;
    
}


void updateMusic()
{
    if(musicState == 1)
    {
        musicSilenceWait -= 1.0f / SCREEN_FPS;
        if(musicSilenceWait <= 0)
        {
            float inDuration = randomRange(MUSIC_IN_MIN, MUSIC_IN_MAX);
            float sustainDuration = randomRange(MUSIC_SUSTAIN_MIN, MUSIC_SUSTAIN_MAX);
            float outDuration = randomRange(MUSIC_OUT_MIN, MUSIC_OUT_MAX);
            
            playSound(musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
            playSound(musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
            playSound(musicChannelNoiseHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
            playSound(musicChannelNoiseLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    
            musicState = 0;
        }
    }
    
    if(!isSoundPlaying(musicChannelHigh) && !isSoundPlaying(musicChannelLow) &&
       !isSoundPlaying(musicChannelNoiseHigh) && !isSoundPlaying(musicChannelNoiseLow))
    {
        if(musicState == 0)
        {
            musicSilenceWait = randomRange(MUSIC_WAIT_MIN, MUSIC_WAIT_MAX);
            musicState = 1;
        }
        
    }
    
}

void finishMusic()
{
    releaseChannel(musicChannelHigh);
    releaseChannel(musicChannelLow);
    releaseChannel(musicChannelNoiseHigh);
    releaseChannel(musicChannelNoiseLow);
}
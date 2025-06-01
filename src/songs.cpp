#include <songs.hpp>


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

int musicSoundHigh;
int musicSoundNoiseHigh;

int musicSoundLow;
int musicSoundNoiseLow;

Song songs[MAX_SONGS];

int songPlayState;

int songPlayingIndex;
float songPlayingTimer;
int songPlayingRow;
int songPlayingChannels[MAX_CHANNELS_PER_SONG];
int songPlayingSounds[MAX_CHANNELS_PER_SONG];



int songPlayerCallback( const void *input, void *output, unsigned long sampleIndex, unsigned long totalSamples, void *callbackData )
{    
    int isPlayState = (songPlayState == PLAYSTATE_PLAYING);
    
    if(isPlayState)
    {
        songPlayingTimer += (1.0f / SAMPLE_RATE);
        
        float rowDuration = songs[songPlayingIndex].rowDuration;
        
        while(songPlayingTimer >= rowDuration)
        {
            for(int c = 0; c < MAX_CHANNELS_PER_SONG; c++)
            {
                if(songs[songPlayingIndex].sounds[c][songPlayingRow].frequency > 0)
                {
                    SongSound* p = &songs[songPlayingIndex].sounds[c][songPlayingRow];
                    
                    logMessageVar("Playing song row ", songPlayingRow);
                    logMessageVar("Playing song channel ", c);
                    
                    
                    playSound(songPlayingSounds[c], songPlayingChannels[c], p->frequency, p->volume, p->attackDuration * rowDuration, p->sustainDuration * rowDuration, p->decayDuration * rowDuration, 0);

                    // char message[100];
                    // sprintf(message, "S %d C %d f%f v%f at%f st%f dt%f", playSounds[c], playChannels[c], p->frequency, p->volume, p->attackDuration * 1.0f/bps, p->sustainDuration * 1.0f/bps, p->decayDuration * 1.0f/bps);
                    // logMessage(message);
                }

            }
            
            songPlayingRow ++;
            
            int rangeStart = songs[songPlayingIndex].rangeStartRow;
            int rangeCount = songs[songPlayingIndex].rangeRowsCount;
            
            int rangeEnd = rangeStart + rangeCount - 1;
            
            if(songPlayingRow >= MAX_ROWS_PER_SONG)
            {
                songPlayingRow = 0;
            }
                
            if(songPlayingRow > rangeEnd)
            {
                songPlayingRow = rangeStart;
            }
            
            songPlayingTimer -= rowDuration;
        }
    }

    
    return 0;
}


void initSongs()
{
    for(int i = 0; i < MAX_SONGS; i++)
    {
        int success;
        
        char fileName[MAX_PATH_LENGTH];
        sprintf(fileName, SOUND_FILE_PATTERN, i);
        char filePath[MAX_PATH_LENGTH];
        sprintf(filePath, "%s\\%s", SOUND_DIRECTORY, fileName);
        
        FILE* file = fopen(filePath, "rb");
        
        if(file != 0)
        {
            logMessage("********** Loading song *************");
            logMessageVar("Song index", i);
            
            // Tempo
            fread(&songs[i].rowDuration, sizeof(float), 1, file);        
            
            logMessageVar("Row duration", songs[i].rowDuration);

            // Meter
            fread(&songs[i].meter, sizeof(int), 1, file); 

            logMessageVar("Meter", songs[i].meter);
            
            // Num channels
            
            int fileChannels;
            fread(&fileChannels, sizeof(int), 1, file);
            
            ASSERT(fileChannels == MAX_CHANNELS_PER_SONG, "Song file channels count doesn't match")
            
            logMessageVar("Channels", fileChannels);

            // Channel properties 
            
            for(int c = 0; c < MAX_CHANNELS_PER_SONG; c++)
            {
                logMessageVar("Loading channel", c);

                fread(&songs[i].channels[c].type, sizeof(int), 1, file);
                fread(&songs[i].channels[c].volume, sizeof(float), 1, file);            

                logMessageVar("Song channel type ", songs[i].channels[c].type);                        
                logMessageVar("Song channel volume ", songs[i].channels[c].volume);                        
            }   

            // Num rows

            int fileRows;
            fread(&fileRows, sizeof(int), 1, file);
            
            ASSERT(fileRows == MAX_ROWS_PER_SONG, "Song file rows count doesn't match")
            
            logMessageVar("Rows", fileRows);

            // Range
            
            fread(&songs[i].rangeStartRow, sizeof(int), 1, file);
            fread(&songs[i].rangeRowsCount, sizeof(int), 1, file); 

            logMessageVar("Song range start ", songs[i].rangeStartRow);                        
            logMessageVar("Song range count ", songs[i].rangeRowsCount);                        

            // Row properties
            
            for(int r = 0; r < MAX_ROWS_PER_SONG; r++)
            {
                for(int c = 0; c < MAX_CHANNELS_PER_SONG; c++)
                {
                    fread(&songs[i].sounds[c][r].frequency, sizeof(float), 1, file);
                    fread(&songs[i].sounds[c][r].volume, sizeof(float), 1, file);
                    fread(&songs[i].sounds[c][r].attackDuration, sizeof(float), 1, file);
                    fread(&songs[i].sounds[c][r].sustainDuration, sizeof(float), 1, file);
                    fread(&songs[i].sounds[c][r].decayDuration, sizeof(float), 1, file);
                    
                    if(songs[i].sounds[c][r].frequency > 0)
                    {
                        logMessageVar("Loaded sound at channel ", c);                        
                        logMessageVar("Loaded sound at row ", r);                        
                    }
                }
            }        

            fclose(file);

            logMessageVar("Song loaded ", i);
        }    
        else
        {
            logMessageVar("Song file not found ", i);
            songs[i].rowDuration = 1;
            songs[i].meter = METER_TWO_BY_FOUR;
            songs[i].rangeStartRow = 0;
            songs[i].rangeRowsCount = MAX_ROWS_PER_SONG;
            
            for(int c = 0; c < MAX_CHANNELS_PER_SONG; c++)
            {
                songs[i].channels[c].type = CHANNELTYPE_TONE;
                songs[i].channels[c].volume = 0;
                
                for(int r = 0; r < MAX_ROWS_PER_SONG; r ++)
                {
                    songs[i].sounds[c][r].frequency = 0;
                    songs[i].sounds[c][r].volume = 0;
                    songs[i].sounds[c][r].attackDuration = 0;
                    songs[i].sounds[c][r].sustainDuration = 0;
                    songs[i].sounds[c][r].decayDuration = 0;
                }
            }
            
        }
    
    }
    
    
    // musicChannelHigh = reserveChannel(GENERATOR_TYPE_TONE);
    // musicChannelNoiseHigh = reserveChannel(GENERATOR_TYPE_NOISE);
    // musicChannelLow = reserveChannel(GENERATOR_TYPE_TONE);
    // musicChannelNoiseLow = reserveChannel(GENERATOR_TYPE_NOISE);
    
    // musicSoundHigh = reserveSound();
    // musicSoundNoiseHigh = reserveSound();
    // musicSoundLow = reserveSound();
    // musicSoundNoiseLow = reserveSound();

    
    // musicVolume = 0.3f;
    // musicTimer = 0;


    // float inDuration = randomRange(MUSIC_IN_MIN, MUSIC_IN_MAX);
    // float sustainDuration = randomRange(MUSIC_SUSTAIN_MIN, MUSIC_SUSTAIN_MAX);
    // float outDuration = randomRange(MUSIC_OUT_MIN, MUSIC_OUT_MAX);
    
    // playSound(musicSoundHigh, musicChannelHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    // playSound(musicSoundLow, musicChannelLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    // playSound(musicSoundNoiseHigh, musicChannelNoiseHigh, randomRange(MUSIC_FREQUENCY_HIGH_MIN, MUSIC_FREQUENCY_HIGH_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    // playSound(musicSoundNoiseLow, musicChannelNoiseLow, randomRange(MUSIC_FREQUENCY_LOW_MIN, MUSIC_FREQUENCY_LOW_MAX), musicVolume, inDuration, sustainDuration, outDuration, 0);
    // musicState = 0;
                          
    // musicSilenceWait = 0;

    songPlayState = PLAYSTATE_STOPPED;

    setSoundCallbackFunction(&songPlayerCallback);    
}

void playSong(int index)
{

    ASSERT(songPlayState == PLAYSTATE_STOPPED, "Playing a song without stopping previous")
    
    for(int c = 0; c < MAX_CHANNELS_PER_SONG; c++)
    {
        songPlayingChannels[c] = reserveChannel(songs[index].channels[c].type);
        songPlayingSounds[c] = reserveSound();
        
        setChannelVolume(songPlayingChannels[c], songs[index].channels[c].volume);

    }
    
    songPlayingIndex = index;
    songPlayingRow = 0;
    songPlayingTimer = 0;    

    songPlayState = PLAYSTATE_PLAYING;

}

void stopSong()
{
    ASSERT(songPlayState == PLAYSTATE_PLAYING, "Stopping song but no song is playing")

    songPlayState = PLAYSTATE_STOPPED;

    for(int c = 0; c < MAX_CHANNELS_PER_SONG; c++)
    {
        releaseChannel(songPlayingChannels[c]);
        releaseSound(songPlayingSounds[c]);
    }
 
 }

void updateSongs()
{

}

void finishSongs()
{
    releaseChannel(musicChannelHigh);
    releaseChannel(musicChannelLow);
    releaseChannel(musicChannelNoiseHigh);
    releaseChannel(musicChannelNoiseLow);
    
    releaseSound(musicSoundHigh);
    releaseSound(musicSoundLow);
    releaseSound(musicSoundNoiseHigh);
    releaseSound(musicSoundNoiseLow);    
}
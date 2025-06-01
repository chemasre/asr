#ifndef _SONGS_HPP_
#define _SONGS_HPP_

#include <common_header.hpp>
#include <common_system_header.hpp>

#define MAX_SONGS 256
#define MAX_CHANNELS_PER_SONG 16
#define MAX_ROWS_PER_SONG 1024

#define SOUND_DIRECTORY "sounds"
#define SOUND_FILE_PATTERN "sound%03d.snd"

#define PLAYSTATE_STOPPED 0
#define PLAYSTATE_PLAYING 1

#define METER_TWO_BY_FOUR 0
#define METER_THREE_BY_FOUR 1
#define METER_FOUR_BY_FOUR 2

#define CHANNELTYPE_TONE 0
#define CHANNELTYPE_NOISE 1

struct SongChannel
{
	int type;
	float volume;
};

struct SongSound
{
	float frequency;
	float volume;
	
	float attackDuration;
	float sustainDuration;
	float decayDuration;
		
};

struct Song
{
    float rowDuration; 
    int meter;

    int rangeStartRow;
    int rangeRowsCount;
    
    SongChannel channels[MAX_CHANNELS_PER_SONG];    
    SongSound sounds[MAX_CHANNELS_PER_SONG][MAX_ROWS_PER_SONG];
    
};

void initSongs();
void playSong(int index);
void stopSong();
void updateSongs();
void finishSongs();

#endif

#include "sound_editor.hpp"
#include "editor_utils.hpp"
#include "sound.hpp"
#include "log.hpp"

#define CELLS_WIDTH 80
#define CELLS_HEIGHT 44

#define FREQUENCY_OPAQUE MAKE_COLOR(255,0,0)
#define RANGES_COLOR_1 MAKE_COLOR(0,255,255)
#define RANGES_COLOR_2 MAKE_COLOR(0,127,127)

#define SOUND_FILE_PATTERN "sound%02d.snd"

#define NOTES_COUNT 12
#define OCTAVES_COUNT 9

#define ROW_PAGES_COUNT 32

#define FREQUENCY_WIDTH NOTES_COUNT
#define FREQUENCY_HEIGHT OCTAVES_COUNT

#define FREQUENCY_POSITION_X (CELLS_WIDTH - FREQUENCY_WIDTH - 2)
#define FREQUENCY_POSITION_Y (4)

#define VOLUME_POSITION_X (CELLS_WIDTH - VOLUME_WIDTH - 2)
#define VOLUME_POSITION_Y (FREQUENCY_POSITION_Y + FREQUENCY_HEIGHT + 2 + 2)

#define VOLUME_COUNT 12

#define VOLUME_WIDTH VOLUME_COUNT
#define VOLUME_HEIGHT 1

#define TIME_PROPERTY_ATTACK 0
#define TIME_PROPERTY_SUSTAIN 1
#define TIME_PROPERTY_DECAY 2

#define TIME_PROPERTY_COUNT 3

#define TIME_PROPERTY_VALUES_COUNT 12

#define TIME_POSITION_X (CELLS_WIDTH - CHANNELTYPE_WIDTH - 2)
#define TIME_POSITION_Y (VOLUME_POSITION_Y  + VOLUME_HEIGHT + 2 + 2)

#define TIME_WIDTH TIME_PROPERTY_VALUES_COUNT
#define TIME_HEIGHT (2 * TIME_PROPERTY_COUNT)

#define CHANNELTYPE_TONE 0
#define CHANNELTYPE_NOISE 1

#define CHANNELTYPE_COUNT 2

#define CHANNELTYPE_WIDTH 12
#define CHANNELTYPE_HEIGHT 1

#define CHANNELTYPE_POSITION_X (CELLS_WIDTH - CHANNELTYPE_WIDTH - 2)
#define CHANNELTYPE_POSITION_Y (TIME_POSITION_Y  + TIME_HEIGHT + 2 + 2)

#define CHANNELVOLUME_POSITION_X (CELLS_WIDTH - CHANNELVOLUME_WIDTH - 2)
#define CHANNELVOLUME_POSITION_Y (CHANNELTYPE_POSITION_Y + CHANNELTYPE_HEIGHT + 2 + 2)

#define CHANNELVOLUME_COUNT 12

#define CHANNELVOLUME_WIDTH CHANNELVOLUME_COUNT
#define CHANNELVOLUME_HEIGHT 1

#define METER_TWO_BY_FOUR 0
#define METER_THREE_BY_FOUR 1
#define METER_FOUR_BY_FOUR 2

#define METER_COUNT 3

#define METER_POSITION_X (CELLS_WIDTH - METER_WIDTH - 2)
#define METER_POSITION_Y (CHANNELVOLUME_POSITION_Y + CHANNELVOLUME_HEIGHT + 2 + 2)

#define METER_WIDTH 12
#define METER_HEIGHT 1

#define TEMPO_COUNT 12

#define TEMPO_POSITION_X (FREQUENCY_POSITION_X - 1 - TEMPO_WIDTH - 2)
#define TEMPO_POSITION_Y (4)

#define TEMPO_WIDTH TEMPO_COUNT
#define TEMPO_HEIGHT (1)



#define TOOL_SELECT 0
#define TOOL_SET 1
#define TOOL_PICK 2
#define TOOL_RANGE 3

#define TOOLS_COUNT 4
#define TOOLS_WIDTH 12
#define TOOLS_HEIGHT TOOLS_COUNT
#define TOOLS_POSITION_X 3
#define TOOLS_POSITION_Y 4

#define MODE_ALL   0
#define MODE_FREQUENCY 1
#define MODE_VOLUME 2
#define MODE_TIME 3

#define MODES_COUNT 4

#define MODES_WIDTH 12
#define MODES_HEIGHT MODES_COUNT
#define MODES_POSITION_X 3
#define MODES_POSITION_Y (TOOLS_POSITION_Y + TOOLS_HEIGHT + 5)

#define COMMAND_NEW 0
#define COMMAND_LOAD 1
#define COMMAND_SAVE 2
#define COMMAND_EXIT 3
#define COMMAND_COPY 4
#define COMMAND_CUT 5
#define COMMAND_PASTE 6
#define COMMAND_CLEAR 7
#define COMMAND_SHIFT_UP 8
#define COMMAND_SHIFT_DOWN 9
#define COMMAND_PLAY_OR_STOP 10

#define COMMAND_HIGHLIGHT_TIME (0.5f)

#define COMMANDS1_COUNT 4

#define COMMANDS1_WIDTH 12
#define COMMANDS1_HEIGHT COMMANDS1_COUNT
#define COMMANDS1_POSITION_X 3
#define COMMANDS1_POSITION_Y (MODES_POSITION_Y + MODES_HEIGHT + 5)

#define COMMANDS2_COUNT 7

#define COMMANDS2_WIDTH 12
#define COMMANDS2_HEIGHT COMMANDS2_COUNT
#define COMMANDS2_POSITION_X 3
#define COMMANDS2_POSITION_Y (COMMANDS1_POSITION_Y + COMMANDS1_HEIGHT + 5)

#define SLOTS_COUNT 16
#define SLOTS_WIDTH (SLOTS_COUNT * 2)
#define SLOTS_HEIGHT 1
#define SLOTS_POSITION_X (TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5)
#define SLOTS_POSITION_Y (CELLS_HEIGHT - SLOTS_HEIGHT - 1)

#define PLAYSTATE_STOPPED 0
#define PLAYSTATE_PLAYING 1


char cursorChar;
int cursorColor;
int cursorCellX;
int cursorCellY;

int selectedTool;
int selectedMode;

int selectedSlot;
int selectedNote;
int selectedOctave;
int selectedVolumeLevel;
int selectedAttackTime;
int selectedSustainTime;
int selectedDecayTime;
int selectedChannelType;
int selectedChannelVolume;
int selectedMeter;
int selectedTempo;
int selectedRow;
int selectedRowPage;

int selectionPosX;
int selectionPosY;
int selectionWidth;
int selectionHeight;

int soundAreaPosX;
int soundAreaPosY;
int soundAreaWidth;
int soundAreaHeight;

int soundRowsPosX;
int soundRowsPosY;
int soundRowsWidth;
int soundRowsHeight;

int soundChannelsPosX;
int soundChannelsPosY;
int soundChannelsWidth;
int soundChannelsHeight;

int soundRowPagesPosX;
int soundRowPagesPosY;
int soundRowPagesWidth;
int soundRowPagesHeight;

char noteCharacters[NOTES_COUNT] = { 'C', 'c', 'D', 'd', 'E', 'F', 'f', 'G', 'g', 'A', 'a', 'B' };
char hexaCharacters[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


char workPath[MAX_PATH_LENGTH];


ChannelProperties* channelCells[SLOTS_COUNT];
SoundProperties** soundCells[SLOTS_COUNT];
int songRanges[SLOTS_COUNT * 2];

SoundProperties** copyBuffer;
int copyBufferPosX;
int copyBufferPosY;
int copyBufferWidth;
int copyBufferHeight;
int copyBufferHasFrequency;
int copyBufferHasVolume;
int copyBufferHasTime;

float commandHighlightedTimer;
int commandHighlighted;
int commandHighlightedError;

int playState;

int* playChannels;
int* playSounds;
int playStartRow;
int playStartRowPage;

float playTimer;

float frequenciesTable[NOTES_COUNT][OCTAVES_COUNT] = 
{
    { 16.35f,   32.70f,      65.41f,    130.81f,    261.63f,    523.25f,    1046.50f,   2093.00f,   4186.01f },
    { 17.32f,   34.65f,      69.30f,    138.59f,    277.18f,    554.37f,    1108.73f,   2217.46f,   4434.92f }, 
    { 18.35f,   36.71f,      73.42f,    146.83f,    293.66f,    587.33f,    1174.66f,   2349.32f,   4698.63f }, 
    { 19.45f,   38.89f,      77.78f,    155.56f,    311.13f,    622.25f,    1244.51f,   2489.02f,   4978.03f }, 
    { 20.60f,   41.20f,      82.41f,    164.81f,    329.63f,    659.25f,    1318.51f,   2637.02f,   5274.04f }, 
    { 21.83f,   43.65f,      87.31f,    174.61f,    349.23f,    698.46f,    1396.91f,   2793.83f,   5587.65f }, 
    { 23.12f,   46.25f,      92.50f,    185.00f,    369.99f,    739.99f,    1479.98f,   2959.96f,   5919.91f }, 
    { 24.50f,   49.00f,      98.00f,    196.00f,    392.00f,    783.99f,    1567.98f,   3135.96f,   6271.93f }, 
    { 25.96f,   51.91f,     103.83f,    207.65f,    415.30f,    830.61f,    1661.22f,   3322.44f,   6644.88f }, 
    { 27.50f,   55.00f,     110.00f,    220.00f,    440.00f,    880.00f,    1760.00f,   3520.00f,   7040.00f },
    { 29.14f,   58.27f,     116.54f,    233.08f,    466.16f,    932.33f,    1864.66f,   3729.31f,   7458.62f }, 
    { 30.87f,   61.74f,     123.47f,    246.94f,    493.88f,    987.77f,    1975.53f,   3951.07f,   7902.13f }
    
};


int getChannelColor(int index, int isHeader)
{
	int color;
	
	float v = channelCells[selectedSlot][index].volume;
	int c1 = (int)(63 + v * 64.0f);
	int c2 = (int)(63 + v * (isHeader ? 128.0f : 64.0f));
	
	if(channelCells[selectedSlot][index].type == CHANNELTYPE_TONE) { color = MAKE_COLOR(c2, 0, c2); }
	else // channelCells[selectedSlot][index].type == CHANNELTYPE_NOISE
	{ color = MAKE_COLOR(c2, c2, 0); }
	
	return color;
}


float getVolume(int level)
{
	return level * (1.0f / (VOLUME_COUNT - 1));
}

int getVolumeLevel(float volume)
{
	int level = 0;
	float distance = FLT_MAX;
	
	for(int i = 0; i < VOLUME_COUNT; i++)
	{
		float d = fabs(volume - i * (1.0f / (VOLUME_COUNT - 1)));
		if(d < distance)
		{
			distance = d;
			level = i;
		}
	}
	
	return level;
}

float getTimeProperty(int level)
{
	return level * (1.0f / (TIME_PROPERTY_VALUES_COUNT - 1));	
}

int getTimePropertyLevel(float value)
{
	int level = 0;
	float distance = FLT_MAX;
	
	for(int i = 0; i < TIME_PROPERTY_VALUES_COUNT; i++)
	{
		float d = fabs(value - i * (1.0f / (TIME_PROPERTY_VALUES_COUNT - 1)));
		if(d < distance)
		{
			distance = d;
			level = i;
		}
	}
	
	return level;
	
}

float getFrequency(int octave, int note)
{
	return frequenciesTable[note][octave];
}

int getNote(float frequency)
{
	int note = 0;
	float distance = FLT_MAX;
	
	for(int o = 0; o < OCTAVES_COUNT; o ++)
	{
		for(int n = 0; n < NOTES_COUNT; n++)
		{
			float d = fabs(frequency - frequenciesTable[n][o]);
			if(d < distance)
			{
				distance = d;
				note = n;
			}
		}
	}
		
	return note;
}

int getOctave(float frequency)
{
	int octave = 0;
	float distance = FLT_MAX;
	
	for(int o = 0; o < OCTAVES_COUNT; o ++)
	{
		for(int n = 0; n < NOTES_COUNT; n++)
		{
			float d = fabs(frequency - frequenciesTable[n][o]);
			if(d < distance)
			{
				distance = d;
				octave = o;
			}
		}
	}
	
	return octave;
}

int soundEditorCallback( const void *input, void *output, unsigned long sampleIndex, unsigned long totalSamples, void *callbackData )
{    
    int isPlayState = (playState == PLAYSTATE_PLAYING);
    
    if(isPlayState)
    {
        int bps = (selectedTempo + 1);
        playTimer += (1.0f / SAMPLE_RATE);
        
        while(playTimer >= 1.0f / bps)
        {
            for(int c = 0; c < soundAreaWidth; c++)
            {
                if(soundCells[selectedSlot][selectedRow][c].frequency > 0)
                {
                    SoundProperties* p = &soundCells[selectedSlot][selectedRow][c];
                    
                    playSound(playSounds[c], playChannels[c], p->frequency, p->volume, p->attackDuration * 1.0f/bps, p->sustainDuration * 1.0f/bps, p->decayDuration * 1.0f/bps, 0);

                    // char message[100];
                    // sprintf(message, "S %d C %d f%f v%f at%f st%f dt%f", playSounds[c], playChannels[c], p->frequency, p->volume, p->attackDuration * 1.0f/bps, p->sustainDuration * 1.0f/bps, p->decayDuration * 1.0f/bps);
                    // logMessage(message);
                }

            }
            
            selectedRow ++;
            
            int rangeStart = songRanges[selectedSlot * 2 + 0];
            int rangeCount = songRanges[selectedSlot * 2 + 1];
            int rangeEnd = rangeStart + rangeCount - 1;
            int rangeStartPage = rangeStart / soundAreaHeight;
            int rangeStartRow = rangeStart % soundAreaHeight;
            int rangeEndPage = rangeEnd / soundAreaHeight;
            int rangeEndRow = rangeEnd % soundAreaHeight;

            
            if(selectedRow >= soundAreaHeight)
            {
                selectedRow = 0;
                selectedRowPage ++;
            }
                
            if(selectedRowPage >= ROW_PAGES_COUNT ||
               selectedRowPage > rangeEndPage ||
               selectedRowPage == rangeEndPage && selectedRow > rangeEndRow)
            {
                selectedRowPage = rangeStartPage;
                selectedRow = rangeStartRow;
            }
            
            playTimer -= 1.0f / bps;
        }
    }

    
    return 0;
}


void drawUI()
{
	int isPlayState = (playState == PLAYSTATE_PLAYING);
	
    drawWindow(FREQUENCY_POSITION_X - 1, FREQUENCY_POSITION_Y - 3, FREQUENCY_WIDTH + 2, FREQUENCY_HEIGHT + 4, "NOTE", MAKE_COLOR(255, 255, 0));    

    drawWindow(VOLUME_POSITION_X - 1, VOLUME_POSITION_Y - 3, VOLUME_WIDTH + 2, VOLUME_HEIGHT + 4, "VOLUME", MAKE_COLOR(255, 255, 0));    

    drawWindow(TIME_POSITION_X - 1, TIME_POSITION_Y - 3, TIME_WIDTH + 2, TIME_HEIGHT + 4, "TIME", MAKE_COLOR(255, 255, 0));  
    
	drawWindow(CHANNELTYPE_POSITION_X - 1, CHANNELTYPE_POSITION_Y - 3, CHANNELTYPE_WIDTH + 2, CHANNELTYPE_HEIGHT + 4, "CH TYPE", MAKE_COLOR(255, 255, 0));
  
    drawWindow(CHANNELVOLUME_POSITION_X - 1, CHANNELVOLUME_POSITION_Y - 3, CHANNELVOLUME_WIDTH + 2, CHANNELVOLUME_HEIGHT + 4, "CH VOLUME", MAKE_COLOR(255, 255, 0));
	
    drawWindow(METER_POSITION_X - 1, METER_POSITION_Y - 3, METER_WIDTH + 2, METER_HEIGHT + 4, "METER", MAKE_COLOR(255, 255, 0));  

    drawWindow(TEMPO_POSITION_X - 1, TEMPO_POSITION_Y - 3, TEMPO_WIDTH + 2, TEMPO_HEIGHT + 4, "TEMPO", MAKE_COLOR(255, 255, 0));  

    drawWindow(soundAreaPosX - 1, soundAreaPosY - 3 - 2, soundAreaWidth + 2 + soundRowsWidth + 1, soundAreaHeight + 4 + 2, "CHANNELS", MAKE_COLOR(255, 255, 0)); 
	
	fillScreenArea(soundAreaPosX, soundAreaPosY - 1, soundAreaWidth + 1 + soundRowsWidth, 1, MAKE_COLOR(255,255,0), '-');

	setScreenCell(soundAreaPosX + soundAreaWidth, soundAreaPosY - 2, MAKE_COLOR(255,255,0), '|');

	drawString(!isPlayState ? MAKE_COLOR(255,255,255):COLOR_UNSELECTED, "- ", soundAreaPosX + soundAreaWidth + 1, soundAreaPosY - 2);
	drawString(!isPlayState ? MAKE_COLOR(255,255,255):COLOR_UNSELECTED, " +", soundAreaPosX + soundAreaWidth + 3, soundAreaPosY - 2);

	fillScreenArea(soundAreaPosX + soundAreaWidth, soundAreaPosY, 1, soundAreaHeight, MAKE_COLOR(255,255,0), '|');

    drawWindow(SLOTS_POSITION_X - 1, SLOTS_POSITION_Y - 1, SLOTS_WIDTH + 2, SLOTS_HEIGHT + 2, NULL, MAKE_COLOR(255, 255, 0));
    
    drawWindow(TOOLS_POSITION_X - 1, TOOLS_POSITION_Y - 3, TOOLS_WIDTH + 2, TOOLS_HEIGHT + 4, "TOOLS", MAKE_COLOR(255, 255, 0));    

    drawWindow(MODES_POSITION_X - 1, MODES_POSITION_Y - 3, MODES_WIDTH + 2, MODES_HEIGHT + 4, "MODES", MAKE_COLOR(255, 255, 0));    

    drawWindow(COMMANDS1_POSITION_X - 1, COMMANDS1_POSITION_Y - 3, COMMANDS1_WIDTH + 2, COMMANDS1_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 
        
    drawWindow(COMMANDS2_POSITION_X - 1, COMMANDS2_POSITION_Y - 3, COMMANDS2_WIDTH + 2, COMMANDS2_HEIGHT + 4, "COMMAND", MAKE_COLOR(255, 255, 0)); 
    
    
    for(int y = 0; y < FREQUENCY_HEIGHT; y++)
    {
        for(int x = 0; x < FREQUENCY_WIDTH; x++)
        {
            int r = (int)(x * (255.0f / (NOTES_COUNT - 1)));
            int g = 127 + (int)(selectedVolumeLevel * (128.0f / (VOLUME_COUNT - 1)));
            int b = (int)(int)(y * (255.0f / (OCTAVES_COUNT - 1)));
            
            int color;
            if(selectedNote == x && selectedOctave == y) { color = COLOR_SELECTED; }
            else { color = MAKE_COLOR(r, g, b); }            
            
            setScreenCell(FREQUENCY_POSITION_X + x, FREQUENCY_POSITION_Y + y, color, noteCharacters[x]);
        }
    }
	
	for(int x = 0; x < soundChannelsWidth; x++)
	{
		int color = getChannelColor(x, 1);		
		
		setScreenCell(soundChannelsPosX + x, soundChannelsPosY, color, hexaCharacters[x]);
	}
	
    
    for(int x = 0; x < VOLUME_WIDTH; x++)
    {
        int r = 128;
        int g = (int)(x * (255.0f / VOLUME_WIDTH));
        int b = 128;
        
        int color;
        if(selectedVolumeLevel == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 
        
        setScreenCell(VOLUME_POSITION_X + x, VOLUME_POSITION_Y, color, '#');
    }
	
	drawString(MAKE_COLOR(255,255,255), "ATTACK", TIME_POSITION_X, TIME_POSITION_Y + (TIME_PROPERTY_ATTACK) * 2);
	drawString(MAKE_COLOR(255,255,255), "SUSTAIN", TIME_POSITION_X, TIME_POSITION_Y + (TIME_PROPERTY_SUSTAIN) * 2);
	drawString(MAKE_COLOR(255,255,255), "DECAY", TIME_POSITION_X, TIME_POSITION_Y + (TIME_PROPERTY_DECAY) * 2);

    for(int x = 0; x < TIME_WIDTH; x++)
    {
        int r = (int)(x * (255.0f / TIME_WIDTH));
        int g = 128;
        int b = 128;
        
        int color;
        if(selectedAttackTime == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 

        setScreenCell(TIME_POSITION_X + x, TIME_POSITION_Y + (TIME_PROPERTY_ATTACK) * 2 + 1, color, '#');

        r = 128;
        g = 128;
        b = (int)(x * (255.0f / TIME_WIDTH));

        if(selectedSustainTime == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 

        setScreenCell(TIME_POSITION_X + x, TIME_POSITION_Y + (TIME_PROPERTY_SUSTAIN) * 2 + 1, color, '#');

        r = 128;
        g = (int)(x * (255.0f / TIME_WIDTH));
        b = (int)(x * (255.0f / TIME_WIDTH));

        if(selectedDecayTime == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 

        setScreenCell(TIME_POSITION_X + x, TIME_POSITION_Y + (TIME_PROPERTY_DECAY) * 2 + 1, color, '#');
    } 
	

    drawString(selectedChannelType == 0 && !isPlayState ? COLOR_SELECTED : COLOR_UNSELECTED, "TONE", CHANNELTYPE_POSITION_X, CHANNELTYPE_POSITION_Y);
    drawString(selectedChannelType == 1 && !isPlayState ? COLOR_SELECTED : COLOR_UNSELECTED, "NOISE", CHANNELTYPE_POSITION_X + CHANNELTYPE_WIDTH / 2, CHANNELTYPE_POSITION_Y);

    drawString(selectedMeter == METER_TWO_BY_FOUR ? COLOR_SELECTED : COLOR_UNSELECTED, "2/4", METER_POSITION_X, METER_POSITION_Y);
    drawString(selectedMeter == METER_THREE_BY_FOUR ? COLOR_SELECTED : COLOR_UNSELECTED, "3/4", METER_POSITION_X + METER_WIDTH / METER_COUNT, METER_POSITION_Y);
    drawString(selectedMeter == METER_FOUR_BY_FOUR ? COLOR_SELECTED : COLOR_UNSELECTED, "4/4", METER_POSITION_X + METER_WIDTH / METER_COUNT * 2, METER_POSITION_Y);
	
    for(int x = 0; x < TEMPO_WIDTH; x++)
    {
        int r = 128;
        int g = (int)(x * (255.0f / TEMPO_WIDTH));
        int b = 128;
        
        int color;
        if(selectedTempo == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 
        
        setScreenCell(TEMPO_POSITION_X + x, TEMPO_POSITION_Y, color, hexaCharacters[x]);
    }


    for(int x = 0; x < CHANNELVOLUME_WIDTH; x++)
    {
        int r = (int)(x * (255.0f / CHANNELVOLUME_WIDTH));
        int g = 64;
        int b = 64;
        
        int color;
        if(selectedChannelVolume == x) { color = COLOR_SELECTED; }
        else { color = MAKE_COLOR(r, g, b); } 
        
        setScreenCell(CHANNELVOLUME_POSITION_X + x, CHANNELVOLUME_POSITION_Y, color, '#');
    }	
	
    for(int y = 0; y < soundAreaHeight; y++)
    {
        for(int x = 0; x < soundAreaWidth; x ++)
        {
			int pageRow = y;
			int row = selectedRowPage * soundRowsHeight + pageRow;

			int rangeStart = songRanges[selectedSlot * 2 + 0];
			int rangeCount = songRanges[selectedSlot * 2 + 1];
			int rangeEnd = rangeStart + rangeCount - 1;		
			
			char c = '|';
			if(row == selectedRow) { c = '-'; }
			else if(row == rangeStart || row == rangeEnd) { c = '-'; }
			else if(selectedMeter == METER_TWO_BY_FOUR && row % 2 == 0) { c = '-'; } 
			else if(selectedMeter == METER_THREE_BY_FOUR && row % 3 == 0) { c = '-'; }
			else if(selectedMeter == METER_FOUR_BY_FOUR && row % 4 == 0) { c = '-'; }
			
			int color;
			
			if(pageRow == selectedRow) { color = COLOR_SELECTED; }
			else if(row == rangeStart || row == rangeEnd) { color = RANGES_COLOR_1; }
			else { color = getChannelColor(x, 0); }

			setScreenCell(soundAreaPosX + x, soundAreaPosY + y, color, c);
		}
	}

    for(int y = 0; y < soundAreaHeight; y++)
    {
        for(int x = 0; x < soundAreaWidth; x ++)
        {
            if(soundCells[selectedSlot][y][x].frequency != 0)
            {
				int note = getNote(soundCells[selectedSlot][y][x].frequency);
				int octave = getOctave(soundCells[selectedSlot][y][x].frequency);
				int volumeLevel = getVolumeLevel(soundCells[selectedSlot][y][x].volume);
				
				int r = (int)(note * (255.0f / (NOTES_COUNT - 1)));
				int g = 127 + (int)(volumeLevel * (128.0f / (VOLUME_COUNT - 1)));
				int b = (int)(octave * (255.0f / (OCTAVES_COUNT - 1)));
								
                setScreenCell(soundAreaPosX + x, soundAreaPosY + y, MAKE_COLOR(r,g,b), noteCharacters[note]);
            }
        }
    }
	
	for(int y = 0; y < soundRowsHeight; y ++)
	{
		int color = COLOR_UNSELECTED;
		int pageRow = y;
		int row = pageRow + selectedRowPage * soundRowsHeight;

		if(pageRow == selectedRow) { color = COLOR_SELECTED; }


		for(int x = 0; x < soundRowsWidth; x ++)
		{
			
			int digit = row % 16;
			row = row / 16;
			
			setScreenCell(soundRowsPosX + soundRowsWidth - 1 - x, soundRowsPosY + y, color, hexaCharacters[digit]);
		}
	}

    for(int x = 0; x < SLOTS_COUNT; x ++)
    {
        int color;
        if(x == selectedSlot) { color = COLOR_SELECTED; }
        else { color = COLOR_UNSELECTED; }             

        setScreenCell(SLOTS_POSITION_X + x * 2, SLOTS_POSITION_Y, color, x <= 9 ? '0' + x : 'A' + x - 10);
    }
    
    drawString(selectedTool == TOOL_SELECT ? COLOR_SELECTED : COLOR_UNSELECTED, " SELECT", TOOLS_POSITION_X, TOOLS_POSITION_Y + 0);
    drawString(selectedTool == TOOL_SET ? COLOR_SELECTED : COLOR_UNSELECTED, " SET", TOOLS_POSITION_X, TOOLS_POSITION_Y + 1);
    drawString(selectedTool == TOOL_PICK ? COLOR_SELECTED : COLOR_UNSELECTED, " PICK", TOOLS_POSITION_X, TOOLS_POSITION_Y + 2);
    drawString(selectedTool == TOOL_RANGE ? COLOR_SELECTED : COLOR_UNSELECTED, " RANGE", TOOLS_POSITION_X, TOOLS_POSITION_Y + 3);

    drawString(selectedMode == MODE_ALL ? COLOR_SELECTED : COLOR_UNSELECTED, " ALL", MODES_POSITION_X, MODES_POSITION_Y + 0);
    drawString(selectedMode == MODE_FREQUENCY ? COLOR_SELECTED : COLOR_UNSELECTED, " FREQUENCY", MODES_POSITION_X, MODES_POSITION_Y + 1);
    drawString(selectedMode == MODE_VOLUME ? COLOR_SELECTED : COLOR_UNSELECTED, " VOLUME", MODES_POSITION_X, MODES_POSITION_Y + 2);
    drawString(selectedMode == MODE_TIME ? COLOR_SELECTED : COLOR_UNSELECTED, " DURATION", MODES_POSITION_X, MODES_POSITION_Y + 3);

    int highlight = (commandHighlightedTimer > 0);
    
    int highlightNew = highlight && (commandHighlighted == COMMAND_NEW);
    int highlightLoad = highlight && (commandHighlighted == COMMAND_LOAD);
    int highlightSave = highlight && (commandHighlighted == COMMAND_SAVE);
    int highlightCopy = highlight && (commandHighlighted == COMMAND_COPY);
    int highlightCut = highlight && (commandHighlighted == COMMAND_CUT);
    int highlightPaste = highlight && (commandHighlighted == COMMAND_PASTE);
    int highlightClear = highlight && (commandHighlighted == COMMAND_CLEAR);
    int highlightShiftUp = highlight && (commandHighlighted == COMMAND_SHIFT_UP);
    int highlightShiftDown = highlight && (commandHighlighted == COMMAND_SHIFT_DOWN);
    int highlightPlayOrStop = highlight && (commandHighlighted == COMMAND_PLAY_OR_STOP);
	
    int highlightColor = commandHighlightedError ? COLOR_ERROR : COLOR_SELECTED; 
    
    drawString(!isPlayState ? COLOR_SELECTED : COLOR_UNSELECTED, " F1", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 0);
    drawString(highlightNew ? highlightColor : COLOR_UNSELECTED, "NEW", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 0);

    drawString(!isPlayState ? COLOR_SELECTED : COLOR_UNSELECTED, " F2", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 1);
    drawString(highlightLoad ? highlightColor : COLOR_UNSELECTED, "LOAD", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 1);

    drawString(!isPlayState ? COLOR_SELECTED : COLOR_UNSELECTED, " F3", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 2);
    drawString(highlightSave ? highlightColor : COLOR_UNSELECTED, "SAVE", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 2);

    drawString(COLOR_SELECTED, " ESC", COMMANDS1_POSITION_X, COMMANDS1_POSITION_Y + 3);
    drawString(COLOR_UNSELECTED, "EXIT", COMMANDS1_POSITION_X + 6, COMMANDS1_POSITION_Y + 3);
   
    drawString(COLOR_SELECTED, " CT+C", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 0);
    drawString(highlightCopy ? highlightColor : COLOR_UNSELECTED, "COPY", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 0);
    drawString(COLOR_SELECTED, " CT+X", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 1);
    drawString(highlightCut ? highlightColor : COLOR_UNSELECTED, "CUT", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 1);
    drawString(COLOR_SELECTED, " CT+V", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 2);
    drawString(highlightPaste ? highlightColor : COLOR_UNSELECTED, "PASTE", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 2);
    drawString(COLOR_SELECTED, " C", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 3);
    drawString(highlightClear ? highlightColor : COLOR_UNSELECTED, "CLEAR", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 3);
    drawString(COLOR_SELECTED, " CT+U", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 4);
    drawString(highlightShiftUp ? highlightColor : COLOR_UNSELECTED, "TUNEU", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 4);
    drawString(COLOR_SELECTED, " CT+D", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 5);
    drawString(highlightShiftDown ? highlightColor : COLOR_UNSELECTED, "TUNED", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 5);
    drawString(COLOR_SELECTED, " SPC", COMMANDS2_POSITION_X, COMMANDS2_POSITION_Y + 6);
    drawString(highlightPlayOrStop ? highlightColor : COLOR_UNSELECTED, !isPlayState ? "PLAY" : "STOP", COMMANDS2_POSITION_X + 6, COMMANDS2_POSITION_Y + 6);

}

void setMarqueeTopLeft(int x, int y, int* mX, int* mY, int* mW, int* mH)
{
	if(x <= *mX + *mW - 1)
	{
		*mX = x;
		*mW = *mW - (x - *mX);
		if(*mW <= 0) { printf("NEGATIVE Width"); exit(-1); }
	}
	else
	{
		*mX = x;
		*mW = 1;
	}
	
	if(y <= *mY + *mH - 1)
	{
		*mY  = y;
		*mH = *mH - (y - *mY);
		if(*mH < 0) { printf("NEGATIVE Height"); exit(-1); }
	}
	else
	{
		*mY = y;
		*mH = 1;
	}
	
	if(*mX + *mW - 1 > soundAreaWidth - 1)
	{
		*mW = soundAreaWidth - *mX;		
	}
	
	if(*mY + *mH - 1 > soundAreaHeight - 1)
	{
		*mH = soundAreaHeight - *mY;		
	}
}

void setMarqueeBottomRight(int x, int y, int* mX, int* mY, int* mW, int* mH)
{
	*mW = x < *mX ? 1 : x - *mX + 1;
	*mH = y < *mY ? 1 : y - *mY + 1;                        

}

void intersectMarquee(int &x1, int &y1, int &w1, int &h1, int x2, int y2, int w2, int h2)
{
	
}

void drawMarquee(int mX, int mY, int mW, int mH, int mC)
{
	int dX = soundAreaPosX;
	int dY = soundAreaPosY;
	
	setScreenCell(dX + mX,          dY + mY, mC, '+');
	setScreenCell(dX + mX + mW - 1, dY + mY, mC, '+');
	setScreenCell(dX + mX,          dY + mY + mH - 1, mC, '+');
	setScreenCell(dX + mX + mW - 1, dY + mY + mH - 1, mC, '+');
	
	for(int x = 1; x - 1 < mW - 2; x ++)
	{
		setScreenCell(dX + mX + x, dY + mY, mC, '-');
		setScreenCell(dX + mX + x, dY + mY + mH - 1, mC, '-');
	}
	
	for(int y = 1; y - 1 < mH - 2; y ++)
	{
		setScreenCell(dX + mX,          dY + mY + y, mC, '|');
		setScreenCell(dX + mX + mW - 1, dY + mY + y, mC, '|');
	}

}

int tryLoadSound(int slot)
{
    int success;
    
    char fileName[MAX_PATH_LENGTH];
    sprintf(fileName, SOUND_FILE_PATTERN, slot);
    char filePath[MAX_PATH_LENGTH];
    sprintf(filePath, "%s%s", workPath, fileName);
    
    FILE* file = fopen(filePath, "rb");
    
    if(file != 0)
    {
        fread(&soundAreaWidth, sizeof(int), 1, file);
        fread(&soundAreaHeight, sizeof(int), 1, file);
                
        fread(&songRanges[slot * 2 + 0], sizeof(int), 1, file);
        fread(&songRanges[slot * 2 + 1], sizeof(int), 1, file);
        
        for(int y = 0; y < soundAreaHeight; y++)
        {
            for(int x = 0; x < soundAreaWidth; x++)
            {
                fread(&soundCells[slot][y][x].frequency, sizeof(float), 1, file);
                fread(&soundCells[slot][y][x].volume, sizeof(float), 1, file);
            }
        }

        fclose(file);
        
        success = 1;

    }    
    else
    {
        success = 0;
    }
    
    return success;
}

int trySaveSound(int slot)
{
    int success;
    
    char fileName[MAX_PATH_LENGTH];
    sprintf(fileName, SOUND_FILE_PATTERN, slot);
    char filePath[MAX_PATH_LENGTH];
    sprintf(filePath, "%s%s", workPath, fileName);
    
    FILE* file = fopen(filePath, "wb");
    
    if(file != 0)
    {
        fwrite(&soundAreaWidth, sizeof(int), 1, file);
        fwrite(&soundAreaHeight, sizeof(int), 1, file);
                
        fwrite(&songRanges[slot * 2 + 0], sizeof(int), 1, file);
        fwrite(&songRanges[slot * 2 + 1], sizeof(int), 1, file);
        
        for(int y = 0; y < soundAreaHeight; y++)
        {
            for(int x = 0; x < soundAreaWidth; x++)
            {
                fwrite(&soundCells[slot][y][x].frequency, sizeof(float), 1, file);
                fwrite(&soundCells[slot][y][x].volume, sizeof(float), 1, file);
            }
        }

        fclose(file);
        
        success = 1;
    } 
    else
    {
        success = 0;
    }    
    
    return success;
}

int main(int argc, char* argv[])
{
    if(argc >= 2)
    {
        strcpy(workPath, argv[1]);
    }
    else
    {
        strcpy(workPath, "");
    }
    
    initInput();
	initSound();
    initScreen();
    initUI();
    initMenu();
	initLog();
	
	soundAreaPosX = TOOLS_POSITION_X + TOOLS_WIDTH + 1 + 5;
	soundAreaPosY = 6;
	soundAreaWidth = 16;
	soundAreaHeight = 32;


	soundRowsPosX = soundAreaPosX + soundAreaWidth + 1;
	soundRowsPosY = soundAreaPosY;
	soundRowsWidth = 4;
	soundRowsHeight = soundAreaHeight;
	
	soundChannelsPosX = soundAreaPosX;
	soundChannelsPosY = soundAreaPosY - 2;
	soundChannelsWidth = soundAreaWidth;
	soundChannelsHeight = 1;
	
	soundRowPagesPosX = soundAreaPosX + soundAreaWidth + 1;
	soundRowPagesPosY = soundAreaPosY - 2;
	soundRowPagesWidth = 4;
	soundRowPagesHeight = 1;
	
	char titleString[TITLE_STRING_SIZE];
	sprintf(titleString, TITLE_STRING_PATTERN, "Sound Editor", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, "Jose M Solis");
    setScreenTitle(titleString);
    
    resizeScreen(CELLS_WIDTH * fontWidth, CELLS_HEIGHT * fontHeight);
    
    int quit = 0;
    
    cursorChar = 'O';
    cursorColor = MAKE_COLOR(255, 255, 255);
    
    selectedTool = TOOL_SET;
    selectedMode = MODE_ALL;
    
    selectedSlot = 0;
    selectedNote = 0;
    selectedOctave = 4;
    selectedVolumeLevel = VOLUME_COUNT - 1;
    selectedAttackTime = TIME_WIDTH / 2;
    selectedSustainTime = TIME_WIDTH / 2;
    selectedDecayTime = TIME_WIDTH / 2;
    selectedChannelType = 0;
    selectedChannelVolume = CHANNELVOLUME_COUNT - 1;
	selectedMeter = METER_FOUR_BY_FOUR;
	selectedTempo = TEMPO_COUNT / 2;
	selectedRow = 0;
	selectedRowPage = 0;
	
	selectionPosX = 0;
	selectionPosY = 0;
	selectionWidth = soundAreaWidth;
	selectionHeight = soundAreaHeight;
    
    commandHighlighted = 0;
    commandHighlightedTimer = 0;
    commandHighlightedError = 0;
	
	playState = PLAYSTATE_STOPPED;
	
	playChannels = (int*)malloc(sizeof(int) * soundAreaWidth);
	playSounds = (int*)malloc(sizeof(int) * soundAreaWidth);

    for(int s = 0; s < SLOTS_COUNT; s++)
    {
		channelCells[s] = (ChannelProperties*)malloc(sizeof(ChannelProperties) * soundAreaWidth);
		for(int i = 0; i < soundAreaWidth; i++)
		{
			channelCells[s][i].type = 0;
			channelCells[s][i].volume = 1;			
		}
		
        soundCells[s] = (SoundProperties**)malloc(sizeof(SoundProperties*) * soundAreaHeight);
        for(int i = 0; i < soundAreaHeight; i ++)
        { 
            soundCells[s][i] = (SoundProperties*)malloc(sizeof(SoundProperties) * soundAreaWidth);
            
            for(int j = 0; j < soundAreaWidth; j++)
            {
                soundCells[s][i][j].frequency = 0;
                soundCells[s][i][j].volume = 0;
            }
        }
                
        songRanges[s * 2 + 0] = 0;
        songRanges[s * 2 + 1] = soundAreaHeight;

    }
	
	copyBufferPosX = 0;
	copyBufferPosY = 0;

	copyBufferWidth = 0;
	copyBufferHeight = 0;

	copyBufferHasFrequency = 0;
	copyBufferHasVolume = 0;
	copyBufferHasTime = 0;
    
	copyBuffer = (SoundProperties**)malloc(sizeof(SoundProperties*) * soundAreaHeight);
	for(int y = 0; y < soundAreaHeight; y ++)
	{ 
		copyBuffer[y] = (SoundProperties*)malloc(sizeof(SoundProperties) * soundAreaWidth);
		
		for(int x = 0; x < soundAreaWidth; x++)
		{
			copyBuffer[y][x].frequency = 0;
			copyBuffer[y][x].volume = 0;
		}
	}
    
    for(int i = 0; i < SLOTS_COUNT; i++)
    {
        tryLoadSound(i);
    }
        
    setSoundCallbackFunction(&soundEditorCallback);

    while(!quit)
    {
        int cursorWindowX;
        int cursorWindowY;
        getMousePosition(&cursorWindowX, &cursorWindowY);
        
        cursorCellX = cursorWindowX / fontWidth;
        cursorCellY = cursorWindowY / fontHeight;
		
        int isPlayState = (playState == PLAYSTATE_PLAYING);
			
		int frequencyModeEnabled = (selectedMode == MODE_FREQUENCY || selectedMode == MODE_ALL);
		int volumeModeEnabled = (selectedMode == MODE_VOLUME || selectedMode == MODE_ALL);
		int timeModeEnabled = (selectedMode == MODE_TIME || selectedMode == MODE_ALL);
		
		if(!isKeyPressed(KEY_ALT) && !isPlayState)
		{
			if(isKeyDown(KEY_0)) { selectedSlot = 0; }
			else if(isKeyDown(KEY_1)) { selectedSlot = 1; }
			else if(isKeyDown(KEY_2)) { selectedSlot = 2; }
			else if(isKeyDown(KEY_3)) { selectedSlot = 3; }
			else if(isKeyDown(KEY_4)) { selectedSlot = 4; }
			else if(isKeyDown(KEY_5)) { selectedSlot = 5; }
			else if(isKeyDown(KEY_6)) { selectedSlot = 6; }
			else if(isKeyDown(KEY_7)) { selectedSlot = 7; }
			else if(isKeyDown(KEY_8)) { selectedSlot = 8; }
			else if(isKeyDown(KEY_9)) { selectedSlot = 9; }
		}
		
        if(isKeyDown(KEY_ESC))
        {
            quit = 1;
        }
        else if(isKeyDown(KEY_F1) && !isPlayState)
        {
            for(int y = 0; y < soundAreaHeight; y++)
            {
                for(int x = 0; x < soundAreaWidth; x++)
                {
                    soundCells[selectedSlot][y][x].volume = 0;
                    soundCells[selectedSlot][y][x].frequency = 0;                    
                }
            }
                        
            songRanges[selectedSlot * 2 + 0] = 0;
            songRanges[selectedSlot * 2 + 1] = soundAreaHeight;            
            
            commandHighlighted = COMMAND_NEW;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
            
            
        }
        else if(isKeyDown(KEY_F2) && !isPlayState)
        {
            int success;
            
            success = tryLoadSound(selectedSlot);
            
            commandHighlighted = COMMAND_LOAD;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;

        }        
        else if(isKeyDown(KEY_F3) && !isPlayState)
        {
            int success;
            
            success = trySaveSound(selectedSlot);
            
            commandHighlighted = COMMAND_SAVE;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
        }
		else if(isKeyPressed(KEY_CONTROL) && (isKeyDown(KEY_C) || isKeyDown(KEY_X)))
		{
			int isCut = isKeyPressed(KEY_X);
			
			copyBufferPosX = selectionPosX;
			copyBufferPosY = selectionPosY;
			copyBufferWidth = selectionWidth;
			copyBufferHeight = selectionHeight;
			
			copyBufferHasFrequency = frequencyModeEnabled;
			copyBufferHasVolume = volumeModeEnabled;
			copyBufferHasTime = timeModeEnabled;

			for(int y = 0; y < copyBufferHeight; y ++)
			{
				for(int x = 0; x < copyBufferWidth; x++)
				{
					int f = soundCells[selectedSlot][selectionPosY + y][selectionPosX + x].frequency;					
					float v = soundCells[selectedSlot][selectionPosY + y][selectionPosX + x].volume;
					
					float at = soundCells[selectedSlot][selectionPosY + y][selectionPosX + x].attackDuration;
					float st = soundCells[selectedSlot][selectionPosY + y][selectionPosX + x].sustainDuration;
					float dt = soundCells[selectedSlot][selectionPosY + y][selectionPosX + x].decayDuration;

					if(frequencyModeEnabled)
					{						
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].frequency = f;						
						if(isCut) { soundCells[selectedSlot][y + copyBufferPosY][x + copyBufferPosX].frequency = 0; }
					}
					
					if(volumeModeEnabled)
					{
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].volume = v;
						if(isCut) { soundCells[selectedSlot][y + selectionPosY][x + selectionPosX].volume = 0; }
					}
					
					if(timeModeEnabled)
					{
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].attackDuration = at;
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].sustainDuration = st;
						copyBuffer[y + copyBufferPosY][x + copyBufferPosX].decayDuration = dt;
						if(isCut)
						{
							soundCells[selectedSlot][y + selectionPosY][x + selectionPosX].attackDuration = 0;
							soundCells[selectedSlot][y + selectionPosY][x + selectionPosX].sustainDuration = 0;
							soundCells[selectedSlot][y + selectionPosY][x + selectionPosX].decayDuration = 0;
						}
					}
				}
			}
			
            commandHighlighted = isCut ? COMMAND_CUT : COMMAND_COPY;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
			
				
		}
		else if(isKeyPressed(KEY_CONTROL) && isKeyDown(KEY_V))
		{
			int copyFrequency = frequencyModeEnabled && copyBufferHasFrequency;
			int copyVolume = volumeModeEnabled && copyBufferHasVolume;
			int copyTime = timeModeEnabled && copyBufferHasTime;
			
			int success = 0;
									
			if(copyBufferHasFrequency || copyBufferHasVolume || copyBufferHasTime)				
			{
				for(int y = 0; y < copyBufferHeight; y ++)
				{
					for(int x = 0; x < copyBufferWidth; x++)
					{
						int f = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].frequency;
						float v = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].volume;
						float at = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].attackDuration;
						float st = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].sustainDuration;
						float dt = copyBuffer[copyBufferPosY + y][copyBufferPosX + x].decayDuration;
						
						int soundX = cursorCellX - soundAreaPosX + x;
						int soundY = cursorCellY - soundAreaPosY + y;
						
						if(isInsideRect(soundX, soundY, 0, 0, soundAreaWidth, soundAreaHeight))
						{
							if(copyFrequency)
							{
								soundCells[selectedSlot][soundY][soundX].frequency = f;
							}
							
							if(copyVolume)
							{
								soundCells[selectedSlot][soundY][soundX].volume = v;
							}
														
							if(copyTime)
							{
								soundCells[selectedSlot][soundY][soundX].attackDuration = at;
								soundCells[selectedSlot][soundY][soundX].sustainDuration = st;
								soundCells[selectedSlot][soundY][soundX].decayDuration = dt;
							}

							success = 1;
						}
					}
				}
				
			}
			
			if(success)
			{
				selectionPosX = (cursorCellX - soundAreaPosX);
				selectionPosY = (cursorCellY - soundAreaPosY);
				selectionWidth = copyBufferWidth;
				selectionHeight = copyBufferHeight;
				
				if(selectionPosX < 0)
				{
					selectionWidth += selectionPosX;
					selectionWidth = max(selectionWidth, 1);
					selectionPosX = 0;
				}
				else if(selectionPosX > soundAreaWidth - 1)
				{
					selectionPosX = soundAreaWidth - 1;
				}
				
				if(selectionPosX + selectionWidth - 1 > soundAreaWidth - 1)
				{
					selectionWidth = soundAreaWidth - selectionPosX;
				}

				if(selectionPosY < 0)
				{
					selectionHeight += selectionPosY;
					selectionHeight = max(selectionHeight, 1);
					selectionPosY = 0;
				}
				else if(selectionPosY > soundAreaHeight - 1)
				{
					selectionPosY = soundAreaHeight - 1;
				}
				
				if(selectionPosY + selectionHeight - 1 > soundAreaHeight - 1)
				{ 
					selectionHeight = soundAreaHeight - selectionPosY;
				}					
			}

			
            commandHighlighted = COMMAND_PASTE;
            commandHighlightedError = !success;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
				
		}
		else if(isKeyPressed(KEY_CONTROL) && (isKeyDown(KEY_U) || isKeyDown(KEY_D)))
		{
			int isUp = isKeyPressed(KEY_U);
	
			for(int y = selectionPosY; y < selectionHeight; y ++)
			{
				for(int x = selectionPosX; x < selectionWidth; x++)
				{
					if(frequencyModeEnabled && soundCells[selectedSlot][y][x].frequency > 0)
					{
						int note   = getNote(soundCells[selectedSlot][y][x].frequency);
						int octave = getOctave(soundCells[selectedSlot][y][x].frequency);
                            
                        if(isUp)
                        {
                            note ++;
                            if(note >= NOTES_COUNT)
                            {
                                octave ++;
                                
                                if(octave >= OCTAVES_COUNT)
                                {                                    
                                    note = NOTES_COUNT - 1;
                                    octave = OCTAVES_COUNT - 1;
                                }
                                else
                                {
                                    note = 0;
                                }
                            }
                        }
                        else
                        {
                            note --;
                            
                            if(note < 0)
                            {
                                octave --;
                                
                                if(octave < 0)
                                {
                                    note = 0;
                                    octave = 0;
                                }
                                else
                                {
                                    note = NOTES_COUNT - 1;
                                }
                            }

                        }
                        
                        soundCells[selectedSlot][y][x].frequency = getFrequency(octave, note);
					}
					
					if(volumeModeEnabled)
					{
						int l = getVolumeLevel(soundCells[selectedSlot][y][x].volume);
                        if(isUp) { l++; if(l >= VOLUME_COUNT) { l = VOLUME_COUNT - 1; } }
                        else { l--; if(l < 0) { l = 0; } }
                        
                        soundCells[selectedSlot][y][x].volume = getVolume(l);
                        
					}
					
					if(timeModeEnabled)
					{
						int al = getTimePropertyLevel(soundCells[selectedSlot][y][x].attackDuration);
						int sl = getTimePropertyLevel(soundCells[selectedSlot][y][x].sustainDuration);
						int dl = getTimePropertyLevel(soundCells[selectedSlot][y][x].decayDuration);
                        
                        if(isUp) { al++; if(al >= TIME_PROPERTY_VALUES_COUNT) { al = TIME_PROPERTY_VALUES_COUNT - 1; } }
                        else { al--; if(al < 0) { al = 0; } }                        
                        
                        if(isUp) { sl++; if(sl >= TIME_PROPERTY_VALUES_COUNT) { sl = TIME_PROPERTY_VALUES_COUNT - 1; } }
                        else { sl--; if(sl < 0) { sl = 0; } }                        

                        if(isUp) { dl++; if(dl >= TIME_PROPERTY_VALUES_COUNT) { dl = TIME_PROPERTY_VALUES_COUNT - 1; } }
                        else { dl--; if(dl < 0) { dl = 0; } }                        
                        
                        soundCells[selectedSlot][y][x].attackDuration = getTimeProperty(al);
                        soundCells[selectedSlot][y][x].sustainDuration = getTimeProperty(sl);
                        soundCells[selectedSlot][y][x].decayDuration = getTimeProperty(dl);
					}
				}
			}					

            commandHighlighted = isUp ? COMMAND_SHIFT_UP : COMMAND_SHIFT_DOWN;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
		}
		else if(!isKeyPressed(KEY_CONTROL) && isKeyDown(KEY_C))
		{
			for(int y = 0; y < selectionHeight; y ++)
			{
				for(int x = 0; x < selectionWidth; x++)
				{
					int cellX = selectionPosX + x;
					int cellY = selectionPosY + y;
										
					if(frequencyModeEnabled)
					{						
						soundCells[selectedSlot][cellY][cellX].frequency = 0;
					}
					
					if(volumeModeEnabled)
					{
						soundCells[selectedSlot][cellY][cellX].volume = 0;
					}
					
					if(timeModeEnabled)
					{
						soundCells[selectedSlot][cellY][cellX].attackDuration = 0;
						soundCells[selectedSlot][cellY][cellX].sustainDuration = 0;
						soundCells[selectedSlot][cellY][cellX].decayDuration = 0;
					}
				}
			}				

            commandHighlighted = COMMAND_CLEAR;
            commandHighlightedError = 0;
            commandHighlightedTimer = COMMAND_HIGHLIGHT_TIME;
		}
		else if(isKeyDown(KEY_SPACE))
		{
			if(playState == PLAYSTATE_STOPPED)
			{
				playState = PLAYSTATE_PLAYING;
												
				int playStartRow = selectedRow;
				int playStartRowPage = selectedRowPage;
				
				
				
				for(int i = 0; i < soundAreaWidth; i++)
				{
					playChannels[i] = reserveChannel(channelCells[selectedSlot][i].type);
					playSounds[i] = reserveSound();
					
					setChannelVolume(playChannels[i], channelCells[selectedSlot][i].volume);

				}
				playTimer = 0;
			}
			else
			{
				for(int i = 0; i < soundAreaWidth; i++)
				{
					releaseChannel(playChannels[i]);
					releaseSound(playSounds[i]);
				}

				playState = PLAYSTATE_STOPPED;
				selectedRow = playStartRow;
				selectedRowPage = playStartRowPage;
			}

		}
        
        int mouseLeftPressed = isKeyPressed(MOUSE_LEFT) ? 1 : 0;
        int mouseRightPressed = isKeyPressed(MOUSE_RIGHT) ? 1 : 0;
		int mouseLeftDown = isKeyDown(MOUSE_LEFT) ? 1 : 0;
                
        if(mouseLeftPressed) { cursorChar = 'X'; }
        else { cursorChar = 'O'; }        

        if(isInsideRect(cursorCellX, cursorCellY,
                        VOLUME_POSITION_X, VOLUME_POSITION_Y,
                        VOLUME_WIDTH, VOLUME_HEIGHT))
        {
            if(mouseLeftPressed)
            {
                selectedVolumeLevel = cursorCellX - VOLUME_POSITION_X;
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        soundChannelsPosX, soundChannelsPosY,
                        soundChannelsWidth, soundChannelsHeight))
        {
            if(mouseLeftPressed)
            {
				if(selectedTool == TOOL_SET)
				{
					int channel = cursorCellX - soundChannelsPosX;
					channelCells[selectedSlot][channel].type = selectedChannelType;
					channelCells[selectedSlot][channel].volume = getVolume(selectedChannelVolume);
				}
				else if(selectedTool == TOOL_PICK)
				{
					int channel = cursorCellX - soundChannelsPosX;
					selectedChannelType = channelCells[selectedSlot][channel].type;
					selectedChannelVolume = getVolumeLevel(channelCells[selectedSlot][channel].volume);
				}
			}
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        soundRowPagesPosX, soundRowPagesPosY,
                        soundRowPagesWidth, soundRowPagesHeight) && !isPlayState)
        {
            if(mouseLeftDown)
            {
                if((cursorCellX - soundRowPagesPosX) % 2 == 0)
				{
					if(selectedRowPage > 0) { selectedRowPage --; }
				}
				else
				{
					if(selectedRowPage < ROW_PAGES_COUNT - 1) { selectedRowPage ++; }
				}
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        soundRowsPosX, soundRowsPosY,
                        soundRowsWidth, soundRowsHeight))
        {
            if(mouseLeftDown)
            {
				selectedRow = cursorCellY - soundRowsPosY;
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        FREQUENCY_POSITION_X, FREQUENCY_POSITION_Y,
                        FREQUENCY_WIDTH, FREQUENCY_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedNote = cursorCellX - FREQUENCY_POSITION_X;
                selectedOctave = cursorCellY - FREQUENCY_POSITION_Y;
            }            
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        TIME_POSITION_X, TIME_POSITION_Y,
                        TIME_WIDTH, TIME_HEIGHT))
		{
            if(mouseLeftPressed)
            {
				int value = cursorCellX - TIME_POSITION_X;
				int property = (cursorCellY - TIME_POSITION_Y) / 2;
			
				if((cursorCellY - TIME_POSITION_Y) % 2 == 1)
				{
					if(property == TIME_PROPERTY_ATTACK) {  selectedAttackTime = value; }
					else if(property == TIME_PROPERTY_SUSTAIN) {  selectedSustainTime = value; }
					else if(property == TIME_PROPERTY_DECAY)
					{  selectedDecayTime = value; }
				}
			
			}			
		}
        else if(isInsideRect(cursorCellX, cursorCellY,
                        soundAreaPosX, soundAreaPosY,
                        soundAreaWidth, soundAreaHeight))
        {
            if(mouseLeftPressed || mouseRightPressed)
            {
                int s = selectedSlot;
                int x = cursorCellX - soundAreaPosX;
                int y = cursorCellY - soundAreaPosY;
                
                int canChangeFrequency = (selectedMode == MODE_ALL || selectedMode == MODE_FREQUENCY);
                int canChangeVolume = (selectedMode == MODE_ALL || selectedMode == MODE_VOLUME);
                int canChangeTime = (selectedMode == MODE_ALL || selectedMode == MODE_TIME);
                
				if(selectedTool == TOOL_SELECT)
				{                        
                    if(mouseLeftPressed)
                    {
						setMarqueeTopLeft(x, y, &selectionPosX, &selectionPosY, &selectionWidth, &selectionHeight);                        
                    }
                    else
                    {
						setMarqueeBottomRight(x, y, &selectionPosX, &selectionPosY, &selectionWidth, &selectionHeight);                        
                    }
										
				}
                if(selectedTool == TOOL_SET)
                {
                    if(mouseLeftPressed)
                    {                          
						if(isInsideRect(x, y, selectionPosX, selectionPosY, selectionWidth, selectionHeight))
						{
							if(canChangeFrequency)
							{
								soundCells[s][y][x].frequency = getFrequency(selectedOctave, selectedNote);
							}
							
							if(canChangeVolume)
							{
								soundCells[s][y][x].volume = getVolume(selectedVolumeLevel);
							}

							if(canChangeTime)
							{
								soundCells[s][y][x].attackDuration = getTimeProperty(selectedAttackTime);
								soundCells[s][y][x].sustainDuration = getTimeProperty(selectedSustainTime);
								soundCells[s][y][x].decayDuration = getTimeProperty(selectedDecayTime);
							}
						}
                        
                    }
                    else
                    {
						if(isInsideRect(x, y, selectionPosX, selectionPosY, selectionWidth, selectionHeight))
						{
							if(canChangeFrequency) { soundCells[s][y][x].frequency = 0; }
							if(canChangeVolume) { soundCells[s][y][x].volume = 0; }
							if(canChangeTime)
							{
								soundCells[s][y][x].attackDuration = 0;
								soundCells[s][y][x].sustainDuration = 0;
								soundCells[s][y][x].decayDuration = 0;
							}
						}                        
                    }
                }
                else if(selectedTool == TOOL_PICK)
                {
                    if(mouseLeftPressed)
                    {                      
                        float f = soundCells[s][y][x].frequency;
                        float v = soundCells[s][y][x].volume;
                        float at = soundCells[s][y][x].attackDuration;
                        float st = soundCells[s][y][x].sustainDuration;
                        float dt = soundCells[s][y][x].decayDuration;
						
                        if(canChangeFrequency)
                        {   selectedNote = getNote(f);
                            selectedOctave = getOctave(f);
                        }
						
                        if(canChangeVolume)
						{
							selectedVolumeLevel = getVolumeLevel(v);
						}

                        if(canChangeTime)
						{
							selectedAttackTime = getTimePropertyLevel(at);
							selectedSustainTime = getTimePropertyLevel(st);
							selectedDecayTime = getTimePropertyLevel(dt);
						}
                    }
                }
                else if(selectedTool == TOOL_RANGE)
                {
                    int rangeStart = songRanges[selectedSlot * 2 + 0];
					int rangeCount = songRanges[selectedSlot * 2 + 1];
					
					int rangeEnd = rangeStart + rangeCount - 1;
                        
                    if(mouseLeftPressed)
                    {
						int nextStart = selectedRowPage * soundAreaHeight + y;
						if(nextStart < rangeEnd)
						{
							rangeCount += (rangeStart - nextStart); 
							rangeStart = nextStart;
						}
						else
						{
							rangeStart = nextStart;
							rangeCount = 1;
						}
					}
                    else
                    {
						int nextEnd = selectedRowPage * soundAreaHeight + y;
						int rangeEnd = rangeStart + rangeCount - 1;
						
						if(nextEnd <= rangeStart)
						{
							rangeStart = nextEnd;
							rangeCount = 1; 
						}
						else
						{
							rangeCount = nextEnd - rangeStart + 1; 
						}
						
                    }
					
                    songRanges[selectedSlot * 2 + 0] = rangeStart;
                    songRanges[selectedSlot * 0 + 1] = rangeCount; 
					
                    
                }

            }

        } 
        else if(isInsideRect(cursorCellX, cursorCellY,
                            SLOTS_POSITION_X, SLOTS_POSITION_Y,
                            SLOTS_WIDTH, SLOTS_HEIGHT) && !isPlayState)
        {
            if(mouseLeftPressed)
            {            
                selectedSlot = (cursorCellX - SLOTS_POSITION_X) / 2;
            }            
        }        
        else if(isInsideRect(cursorCellX, cursorCellY,
                            TOOLS_POSITION_X, TOOLS_POSITION_Y,
                            TOOLS_WIDTH, TOOLS_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedTool = cursorCellY - TOOLS_POSITION_Y;
            }            
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                        MODES_POSITION_X, MODES_POSITION_Y,
                        MODES_WIDTH, MODES_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedMode = cursorCellY - MODES_POSITION_Y;
            }            
        }        
        else if(isInsideRect(cursorCellX, cursorCellY,
                             CHANNELTYPE_POSITION_X, CHANNELTYPE_POSITION_Y,
                             CHANNELTYPE_WIDTH, CHANNELTYPE_HEIGHT) && !isPlayState)
        {
            if(mouseLeftPressed)
            {            
                selectedChannelType = (cursorCellX - CHANNELTYPE_POSITION_X) / (CHANNELTYPE_WIDTH / CHANNELTYPE_COUNT);
            }            
        }        
        else if(isInsideRect(cursorCellX, cursorCellY,
                        CHANNELVOLUME_POSITION_X, CHANNELVOLUME_POSITION_Y,
                        CHANNELVOLUME_WIDTH, CHANNELVOLUME_HEIGHT))
        {
            if(mouseLeftPressed)
            {
                selectedChannelVolume = cursorCellX - CHANNELVOLUME_POSITION_X;
            }
        }
        else if(isInsideRect(cursorCellX, cursorCellY,
                             METER_POSITION_X, METER_POSITION_Y,
                             METER_WIDTH, METER_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedMeter = (cursorCellX - METER_POSITION_X) / (METER_WIDTH / METER_COUNT);
            }            
        }        
        else if(isInsideRect(cursorCellX, cursorCellY,
                             TEMPO_POSITION_X, TEMPO_POSITION_Y,
                             TEMPO_WIDTH, TEMPO_HEIGHT))
        {
            if(mouseLeftPressed)
            {            
                selectedTempo = (cursorCellX - TEMPO_POSITION_X) / (TEMPO_WIDTH / TEMPO_COUNT);
            }            
        }        

        if(commandHighlightedTimer - (1.0f / SCREEN_FPS) >= 0) { commandHighlightedTimer -= (1.0f / SCREEN_FPS); }
        else
        {
            commandHighlighted = 0;
            commandHighlightedError = 0;
            commandHighlightedTimer = 0;
        }
        
        clearScreen();
        
        drawUI();
        		
		int isFullSelection = (selectionPosX == 0 && selectionPosY == 0 && selectionWidth == soundAreaWidth && selectionHeight == soundAreaHeight);
        
		if(selectedTool == TOOL_SELECT || (!isFullSelection && selectedTool == TOOL_SET))
		{
			drawMarquee(selectionPosX, selectionPosY, selectionWidth, selectionHeight, COLOR_SELECTED);			
		}
		
				
		if(selectedTool == TOOL_RANGE)
		{
			int rangeStart = songRanges[selectedSlot * 2 + 0];
			int rangeCount = songRanges[selectedSlot * 2 + 1];
			
			int rangeStartRowPage = rangeStart / soundAreaHeight;
			int rangeStartRow = rangeStart % soundAreaHeight;
			
			int rangeEnd = rangeStart + rangeCount - 1;
			int rangeEndRowPage = rangeEnd / soundAreaHeight;
			int rangeEndRow = rangeEnd % soundAreaHeight;
			

			int rangeStartIndicatorRow;
			char rangeStartIndicatorCharacter = 'S';

			int rangeEndIndicatorRow;
			char rangeEndIndicatorCharacter = 'E';
			
			if(selectedRowPage == rangeStartRowPage && selectedRowPage == rangeEndRowPage && 
				rangeStartRow == rangeEndRow)
			{
				rangeStartIndicatorCharacter = '-';
				rangeEndIndicatorCharacter = '-';
			}

			if(rangeStartRowPage == selectedRowPage) { rangeStartIndicatorRow = rangeStartRow; }
			else if(rangeStartRowPage < selectedRowPage)
			{ rangeStartIndicatorRow = 0; rangeStartIndicatorCharacter = '^'; }
			else // rangeStartRowPage > selectedRowPage
			{ rangeStartIndicatorRow = soundAreaHeight - 1; rangeStartIndicatorCharacter = 'v'; }
			
			if(rangeEndRowPage == selectedRowPage) { rangeEndIndicatorRow = rangeEndRow; }
			else if(rangeEndRowPage < selectedRowPage)
			{ rangeEndIndicatorRow = 0; rangeEndIndicatorCharacter = '^'; }
			else // rangeEndRowPage > selectedRowPage
			{ rangeEndIndicatorRow = soundAreaHeight - 1; rangeEndIndicatorCharacter = 'v'; }
			

			fillScreenArea(soundAreaPosX, soundAreaPosY + rangeEndIndicatorRow, soundAreaWidth, 1, RANGES_COLOR_1, '-');					
			setScreenCell(soundAreaPosX + soundAreaWidth / 2,
							  soundAreaPosY + rangeEndIndicatorRow, RANGES_COLOR_1, rangeEndIndicatorCharacter);
			
			fillScreenArea(soundAreaPosX, soundAreaPosY + rangeStartIndicatorRow, soundAreaWidth, 1, RANGES_COLOR_1, '-');

			setScreenCell(soundAreaPosX + soundAreaWidth / 2,
							  soundAreaPosY + rangeStartIndicatorRow, RANGES_COLOR_1, rangeStartIndicatorCharacter);
		}
        
        setScreenCell(cursorCellX, cursorCellY, cursorColor, cursorChar);
        
        showScreen();
        
        wait((int)(1.0f / SCREEN_FPS * 1000));
    }
	
	finishLog();
    
}
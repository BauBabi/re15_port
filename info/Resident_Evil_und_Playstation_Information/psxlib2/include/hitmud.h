/*

HITMUD MOD stuff

(modified HITMOD4 stuff)

Jum Hig
V0.2

*/

/* HIT4 MOD file format:

Offset	Hex	Value					Num bytes
~~~~~~	~~~	~~~~~					~~~~~~~~~
0	0000	"4mat" (song name)
42	002A	sample 0 length				2
44	002C	sample 0 finetune			1
45	002D	sample 0 volume				1
	..
72	0048	sample 1 length				2
74	004A	sample 1 finetune			1
75	004B	sample 1 volume				1
	..
		sample n data (30 bytes per sample)
	..
	..
950	03B6	Songlength (no. of orders)		1
952	03B8	order list (128 bytes)			128
	..
	..
1080	0438	"HIT4" or "4TIH"			4
1084	043C	Pattern 0 data				1024
	..
	..
	????	Pattern n data				1024
	..
nnnn	nnnn	Sample data				????

					
How to calculate sample data offset:
1. Calculate number of patterns by reading thru order list and
   finding highest pattern number.
2. sample data offset = 1084 + num_patterns * 1024
   (or maybe 1084 + (num_patterns + 1) * 1024) ???


*/

/*	HIT4 row-playing alogorithm
1. Inc tick, if tick = speed then
	2. Get RowPointer
	3. For every channel:
		3.1 if instr != 0 then
			3.1.1 set channel vol from instr
			3.1.2 set channel lastsmp = instr
		3.2 Get note no.
		3.3 If keyon then
			3.3.1 get voice offset for this channel
			3.3.2 set voice vol
			3.3.3 calc pitch
			3.3.4 get lastsmp
			3.3.5 set voice sample offset
			3.3.6 WaitSPU
			3.3.7 update keyon mask
		3.4 RowPointer += 4;
	4. Do keyons
	5. zero tick, update row counter
	6. check for end of pattern, if so:
		6.1 incr order counter
		6.1 get next pattern
		6.2 set RowPointer to new pattern start
	7. check for end of song, if so then loop
  
	That's it. Simple huh?
	6 & 8 track, + other stuff also added.	
*/

// defines
#define NO_EFFECT	0
#define LFO_VIBRATO	1
#define LFO_TREMOLO 2
#define PORTAMENTO	3
#define VOL_SLIDE	4

// HITMOD 4, 6, 8-track runtime data structure
// updated 10/9/1999 to add LFO and slide stuff
typedef struct {
	PsxUInt8 *ModStart;			// start addr of MOD in mem
	PsxUInt32 PattDataSize;		// size of pattern data
	PsxUInt32 SampleEnd;		// addr in SB of end of samples
	PsxUInt32 *RowPointer;		// ptr to current row
	PsxUInt32 SmpAddress[31];	// addr in SB of each sample
	PsxUInt8 NumTracks;			// no of tracks in mod
	PsxUInt8 MasterVol;			// MOD master volume (0-63)
	PsxUInt16 Keys;
	PsxUInt8 Speed;				// ticks / row
	PsxUInt8 Tick;				// current tick
	PsxUInt8 CurrOrder;
	PsxUInt8 CurrRow;
	PsxUInt8 CurrPattern;
	PsxUInt8 SongLength;		// no of orders in song
	PsxUInt8 NumPatterns;		// number of patterns in song
	PsxUInt8 FineTunes[31];		// finetune info for each sample
	PsxUInt8 Volumes[31];		// vol info for each sample
	PsxInt8 ChanVol[8];		// current channel vols
	PsxUInt8 LastSmp[8];		// last sample played on channel
	// updated stuff below:
	PsxUInt8 Effect[8];			// current effect active
	PsxUInt8 LFOptr[8];
	PsxUInt8 LFOSpeed[8];
	PsxUInt8 LFODepth[8];
	PsxUInt16 Period[8];		// last pitch period on this chan
	PsxInt8 PitchSlide[8];		// amt. to slide pitch by
	PsxInt8 VolSlide[8];		// amt. to slide volume by
	PsxUInt8 VolumeLFODepth[8];
	PsxUInt8 PitchLFODepth[8];
} HITMUD;

// ------------------- Function prototypes --------------------

// Initialise MOD structure, upload MOD samples to SPU RAM
// NB: sb_addr is address to upload samples to in SPU RAM
void InitMOD(HITMUD *m, PsxUInt8 *modaddr, PsxUInt32 sb_addr);

// Do next tick of MOD
// Call 50 times per second, using vsync or a timer
int PollMOD(HITMUD *m);

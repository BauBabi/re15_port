// James Font test Playstation proggie

// NB: TODO: add scroller to explain features of font lib.

#include	<syscall.h>
#include	<psxtypes.h>
#include	<int.h>
#include	<gpu.h>
#include	<tmd.h>
#include	<font.h>


#define OT_LENGTH	5
#define LX			64
#define LY			48


static GsOT_TAG zSortTable[1<<OT_LENGTH];
static PACKET cmdBuffer[3000];

// NB: some fonts to use
extern char apple40[];
extern char atarifon[];
extern char gngfont[];
extern char loderun[];
extern char ninjakid[];
extern char xevious[];
extern char luci20aa[];
extern char term1220[];


// define primitives
static BOXFILL myBox;
static CILINE myLine;
static CITRI myTri, myTri2;
static QUAD bgQuad;

static TEXCOORDS tc[256];

int temp;
int temp2;


static void WaitFrames(int frames)
{
	int i;
	for(i=0; i<frames; i++) {
		GPU_VSync();
	}
}


int main(int argc,char **argv)
{

	int i, j, k, n;
	COLOR clr_color = { 0, 32, 96, 0};

	// ordering table
	GsOT    worldOrderTable;
    GsOT *ot;               // pntr to OT

	// what about "ResetCallBack()"

	// install interrupt handler
	INT_SetUpHandler();


	// bypass a lot of probably neccessary stuff
	// and do GPU init stuff
	// check for PAL or NTSC mode.   by ogre.
	if (*(char *)0xbfc7ff52=='E') {
		GPU_SetPAL(1);
		printf("PAL MODE\n");		//PAL MODE
	}
	else {
		GPU_SetPAL(0);
		printf("NTSC MODE\n");		// NTSC MODE
	}
	GPU_Reset(0);		// cold reset ???
	GPU_Init(320, 240, GsNONINTER, 0);
	GPU_DefDispBuff(0, 0, 0, 0);		// was 0 0 0 240

	// set up order table
	worldOrderTable.length = OT_LENGTH;
	worldOrderTable.org = zSortTable;

	// enable display and wait for GPU
	GPU_EnableDisplay(1);
	GPU_DrawSync();


	j = 0; k = 0;

    ot = &worldOrderTable;
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

    GPU_DrawSync();



    // clear buffer
    GPU_SortClear(0, ot, &clr_color);
	// set up the apple ][ font (6x8)
	InitFont((TIMHEADER *)apple40, 6, 8, 16);
	PrintString(ot, 10, 0, 0, "Remember the old APPLE ][ ???");
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

	// set up the atari font
	InitFont((TIMHEADER *)atarifon, 8, 8, 16);
	PrintString(ot, 10, 0, 16, "Or the ATARI 400/800/5200 ???");
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

	// set up the ghosts 'n goblins font
	InitFont((TIMHEADER *)gngfont, 8, 8, 16);
	PrintString(ot, 10, 0, 32, "Here's a font from GHOSTS 'n GOBLINS");
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

	// some more fonts
	InitFont((TIMHEADER *)loderun, 8, 8, 16);
	PrintString(ot, 10, 0, 48, "And one from LODE RUNNER");
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);
    
	InitFont((TIMHEADER *)ninjakid, 8, 8, 16);
	PrintString(ot, 10, 0, 64, "NINJA-KID (a very groovy game)");
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);
    
	InitFont((TIMHEADER *)xevious, 8, 8, 16);
	PrintString(ot, 10, 0, 80, "An all-time classic game, XEVIOUS");
	SetCharSize(16, 16);
	PrintString(ot, 10, 0, 96, "XEVIOUS font Scaled");
	GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

	InitFont((TIMHEADER *)luci20aa, 15, 24, 16);
	PrintString(ot, 10, 0, 120, "Lucida Console 15x24");
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

	InitFont((TIMHEADER *)term1220, 11, 20, 16);
	PrintString(ot, 10, 0, 160, "Terminal 11x20");
	SetCharLight(255,0,128);
	PrintString(ot, 10, 0, 180, "Terminal Shaded");	
    GPU_DrawOt(ot);
    GPU_DrawSync();
    GPU_ClearOt(0, 0, ot);
    GPU_SetCmdWorkSpace(cmdBuffer);

	// Process buffer
    GPU_DrawOt(ot);
    GPU_DrawSync();
	GPU_FlipDisplay();

	// wait a bit
	WaitFrames(2000);

	// wait
	WaitFrames(200);

        INT_ResetHandler();


        temp2 = 27;
        temp = temp2++;
        printf("%u\t",temp);
        printf("Hello there Jamie!\n");


	exit(0);
}

// SPU lib example 4 - play a MOD
//
// The coloured bars show the current order, pattern and channels
//

#include	<syscall.h>
#include	<psxtypes.h>
#include	<int.h>
#include	<gpu.h>
#include	<tmd.h>
#include	<libspu.h>
#include	<hitmud.h>			// required for MOD stuff

// reference the MOD's
extern char larissa[];

#define OT_LENGTH	5


static GsOT_TAG zSortTable[1<<OT_LENGTH];
static PACKET cmdBuffer[1000];

static QUAD rowQuad;
static QUAD pattQuad;
static QUAD noteonQuad;

static HITMUD themod;


int main(void)
{
	int i, j;

	COLOR clrcolor = { 0, 0, 0 };

	// ordering table
	GsOT    worldOrderTable;
	GsOT *ot;               // pntr to OT

	// what about "ResetCallBack()"

	// install interrupt handler
	INT_SetUpHandler();

	// set up quads for debugging mod
	rowQuad.color.r = rowQuad.color.b = 255;
	rowQuad.color.g = 64;
	rowQuad.vert0.x = rowQuad.vert1.x = 8;
	rowQuad.vert0.y = rowQuad.vert2.y = 20;
	rowQuad.vert2.x = rowQuad.vert3.x = 15;
	rowQuad.vert1.y = rowQuad.vert3.y = 147;
	
	pattQuad.color.g = pattQuad.color.b = 255;
	pattQuad.color.r = 64;
	pattQuad.vert0.x = pattQuad.vert1.x = 0;
	pattQuad.vert0.y = pattQuad.vert2.y = 0;
	pattQuad.vert2.x = pattQuad.vert3.x = 7;
	pattQuad.vert1.y = pattQuad.vert3.y = 7;

	// bypass a lot of probably neccessary stuff
	// and do GPU init stuff
	GPU_SetPAL(1);
	GPU_Reset(0);		// cold reset ???
	GPU_Init(320, 240, GsNONINTER, 0);
	GPU_DefDispBuff(0, 0, 0, 240);		// was 0 0 0 240

	GPU_DrawSync();

	// set up order table
	worldOrderTable.length = OT_LENGTH;
	worldOrderTable.org = zSortTable;

	// enable display and wait for GPU
	GPU_EnableDisplay(1);
	GPU_DrawSync();

	ot = &worldOrderTable;
	GPU_ClearOt(0, 0, ot);
	GPU_SetCmdWorkSpace(cmdBuffer);


	// Initialise SPU (may be unneccessary)
	SPU_Reset();
	SPU_Init();
	SPU_Enable();
	i = SPU_Wait();
	printf("SPU_Wait returned %d.\n", i);

	// Initialise the MOD
	// MOD can be 4, 6 or 8 track Protracker module 
	InitMOD(&themod, (unsigned char *)larissa, 0x2000);

	// set MOD master volume to 75%
	themod.MasterVol = 48;

	// preview sample 0
	printf("Preview sample %d ...\n", i);
	// Tell voice 0 where to find th sample
	SPU_VoiceSetData(0, themod.SmpAddress[3]);
	// set voice 0 volume
	SPU_VoiceVol(0, 0x3000, 0x3000);
	// set voice 0 pitch
	SPU_VoicePitch(0, 0x400);
	// play sample
	SPU_VoiceOn(0);
	// wait for 3 seconds
	for(j=0; j<120; j++) GPU_VSync();
	SPU_VoiceOff(0);

	// loop
	while(themod.CurrOrder != 99) {
		ot = &worldOrderTable;
		GPU_ClearOt(0, 0, ot);
		GPU_SetCmdWorkSpace(cmdBuffer);

		GPU_DrawSync();
		GPU_VSync();
		//GPU_FlipDisplay();

		GPU_SortClear(0, ot, &clrcolor);
		GPU_SortQuad(ot, 5, &rowQuad);
		GPU_SortQuad(ot, 5, &pattQuad);

		// show note on's
		for(j=0; j<8; j++) {
			if(themod.Keys & (1 << j)) {
				noteonQuad.color.g = noteonQuad.color.b = 255;
				noteonQuad.color.r = 255;
				noteonQuad.vert0.x = noteonQuad.vert1.x = j*16;
				noteonQuad.vert0.y = noteonQuad.vert2.y = 215 - themod.ChanVol[j];
				noteonQuad.vert2.x = noteonQuad.vert3.x = j*16+15;
				noteonQuad.vert1.y = noteonQuad.vert3.y = 215;
				GPU_SortQuad(ot, 5, &noteonQuad);
			}
			else {
				noteonQuad.color.g = noteonQuad.color.b = 0;
				noteonQuad.color.r = 128;
				noteonQuad.vert0.x = noteonQuad.vert1.x = j*16;
				noteonQuad.vert0.y = noteonQuad.vert2.y = 150;
				noteonQuad.vert2.x = noteonQuad.vert3.x = j*16+15;
				noteonQuad.vert1.y = noteonQuad.vert3.y = 215;
				GPU_SortQuad(ot, 5, &noteonQuad);
			}
		}
			
		// Process buffer
        GPU_DrawOt(ot);
	    PollMOD(&themod);
		rowQuad.vert1.y = rowQuad.vert3.y = 20 + themod.CurrRow * 2;
		pattQuad.vert2.x = pattQuad.vert3.x = 15 + themod.CurrPattern * 8;
		pattQuad.vert2.x = themod.Speed;
		pattQuad.vert3.x = themod.Speed;

	} // end while


	printf("Speed: %d\n", themod.Speed);
	printf("Done.");
	
	exit(0);
}



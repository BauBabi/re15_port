// spulib example 3
// upload sample, upload reverb, set ADSR, visualise ADSR

#include	<syscall.h>
#include	<psxtypes.h>
#include	<int.h>
#include	<gpu.h>
#include	<tmd.h>
#include 	<libspu.h>

extern char jum303[];

#define OT_LENGTH	5

static GsOT_TAG zSortTable[1<<OT_LENGTH];
static PACKET cmdBuffer[100];

static QUAD ADSRQuad;

int main(void)
{
	int i, j, fcount;
	PsxUInt16 adsr;

	COLOR clrcolor = { 0, 0, 0 };

	// ordering table
	GsOT    worldOrderTable;
	GsOT *ot;               // pntr to OT

	// what about "ResetCallBack()"

	// install interrupt handler
	INT_SetUpHandler();


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


	// initialise the SPU
	SPU_Init();
	// turn it on
	SPU_Enable();

	// upload reverb data
	//SPU_LoadReverb(RVB_HALL);
	// set reverb depth (left and right channels)
	//SPU_ReverbDepth(0x3000, 0x5000);

	// upload the sample
	SPU_UploadData(&jum303, 0x1010, 48600);
	// Tell voice 0 where to find the sample
	SPU_VoiceSetData(0, 0x1010);
	// set voice 0 volume
	SPU_VoiceVol(0, 0x3000, 0x3000);
	// set voice 0 sample playback speed
	SPU_VoicePitch(0, 0x1000);
	// set voice 0 ADSR
	SPU_VoiceADSR(0, ATTACK_LINEAR | 66, 15, 10, SUSTAIN_EXP | SUSTAIN_DEC | 70, RELEASE_EXP | 15);
	// turn voice 0 reverb on
	SPU_VoiceReverb(0);
	// play it!
	SPU_VoiceOn(0);


	fcount = 0;
	adsr = 0;
	while(fcount < 640) {
            ot = &worldOrderTable;
            GPU_ClearOt(0, 0, ot);
            GPU_SetCmdWorkSpace(cmdBuffer);

            GPU_DrawSync();
            GPU_VSync();
            //GPU_FlipDisplay();


	    //GPU_SortClear(0, ot, &clrcolor);

		adsr = SPU_GetADSRVol(0);
		ADSRQuad.color.g = ADSRQuad.color.b = 255;
		ADSRQuad.color.r = 255;
		ADSRQuad.vert0.x = ADSRQuad.vert1.x = fcount/2;
		ADSRQuad.vert0.y = ADSRQuad.vert2.y = 200 - (adsr >> 8);
		ADSRQuad.vert2.x = ADSRQuad.vert3.x = fcount/2+1;
		ADSRQuad.vert1.y = ADSRQuad.vert3.y = 200;
		GPU_SortQuad(ot, 5, &ADSRQuad);
			
			// Process buffer
        	GPU_DrawOt(ot);


        // debug
        printf("%d  %d\n", fcount, adsr);
        fcount++;

        if(fcount == 520) SPU_VoiceOff(0);

	}

    SPU_VoiceOff(0);


	printf("Done.");
	
	exit(0);
}



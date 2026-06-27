// James PSX lib version 2 test proggie
// - displays fonts
// - plays MOD file
#include	<syscall.h>
#include	<psxtypes.h>
#include	<int.h>
#include	<gpu.h>
#include	<tmd.h>
#include	<font.h>
#include	<hitmud.h>

#define OT_LENGTH	5
#define LX			64
#define LY			48

static HITMUD themod;

static GsOT_TAG zSortTable[1<<OT_LENGTH];
static PACKET cmdBuffer[3000];

// the font
extern char term1220[];

// the mod
extern char mpatrol[];

// the TMD
extern char star[];

// define primitives
static BOXFILL myBox;
static CILINE myLine;
static CITRI myTri, myTri2;
static QUAD bgQuad;
static QUAD noteonQuad;

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

// convert int to 2-char string
// (no sprintf :( )
void num2str(int num, char *string) {
		string[0] = (num / 10) + 48;
		string[1] = (num % 10) + 48;
		string[2] = 0;
}
	
	
int main(int argc,char **argv)
{

	int	i, j, k, n;
	PsxUInt32 yrot, xrot;
	PsxUInt8 vulevel[8] = { 0, 0, 0, 0, 0, 0, 0 };
	COLOR clr_color = { 0, 0, 0, 0};
	char string[20];
	MATRIX ltm, matrix;
	PsxInt16 sine, cosine;


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
	GTE_Init(320, 240);


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

	// Initialise SPU
	SPU_Reset();
	SPU_Init();
	SPU_Enable();
	i = SPU_Wait();
	printf("SPU_Wait returned %d.\n", i);

	// Initialise the MOD
	// MOD can be 4, 6 or 8 track Protracker module 
	InitMOD(&themod, (unsigned char *)mpatrol, 0x2000);

	// set MOD master volume to 75%
	themod.MasterVol = 48;

	// set up the font
	InitFont((TIMHEADER *)term1220, 11, 20, 16);

	while(1) {

	    ot = &worldOrderTable;
    	GPU_ClearOt(0, 0, ot);
    	GPU_SetCmdWorkSpace(cmdBuffer);

    	// clear buffer
    	GPU_SortClear(0, ot, &clr_color);

        // Rotate TMD about y axis
    	yrot += 16;
        sine = GTE_Sin(yrot);
        cosine = GTE_Cos(yrot);
        GTE_MatrixSetIdentity(&ltm);
        ltm.m[0][0] = ltm.m[2][2] = cosine;
        ltm.m[0][2] = sine;
        ltm.m[2][0] = -sine;

        /* Rotate about x axis */
        xrot += 12;
        sine = GTE_Sin(xrot);
        cosine = GTE_Cos(xrot);
        GTE_MatrixSetIdentity(&matrix);
        matrix.m[1][1] = matrix.m[2][2] = cosine;
        matrix.m[1][2] = sine;
        matrix.m[2][1] = -sine;
        GTE_MatMul(&ltm, &ltm, &matrix);

        // Set translation vector
        ltm.t[0] = 200;
        ltm.t[1] = 0;
        ltm.t[2] = 0x0800;

        // Render TMD
        TMD_Render(ot, (TMDHEADER *)star, &ltm, TRUE);


		SetCharLight(255,255,255);
		PrintString(ot, 10, 0, 8, "Jum's PSX Lib Add-Ons");
		SetCharLight(255,0,128);
		PrintString(ot, 10, 0, 32, "MOD: Moon Patrol Remix");	
		SetCharLight(0,0,240);
		PrintString(ot, 10, 0, 56, "     by Jum");	
		SetCharLight(255,64,0);
		PrintString(ot, 10, 0, 80, "Order:");	
		PrintString(ot, 10, 0, 96, "Pattern:");	
		PrintString(ot, 10, 0, 112,"Row:");	

		// no sprintf() :(
		SetCharLight(255,255,255);		
		num2str(themod.CurrOrder, string);
		PrintString(ot, 10, 100, 80, string);	
		num2str(themod.CurrPattern, string);
		PrintString(ot, 10, 100, 96, string);	
		num2str(themod.CurrRow, string);
		PrintString(ot, 10, 100, 112, string);	
		


		// show note on's
		for(j=0; j<8; j++) {
			if(vulevel[j] != 0) vulevel[j]--;		// update VU meters
			if(themod.Keys & (1 << j)) {
				vulevel[j] = themod.ChanVol[i];
			}
			noteonQuad.color.g = noteonQuad.color.r = vulevel[j] * 3;
			noteonQuad.color.b = 0;
			noteonQuad.vert0.x = noteonQuad.vert1.x = j*16;
			noteonQuad.vert0.y = noteonQuad.vert2.y = 215 - vulevel[j];
			noteonQuad.vert2.x = noteonQuad.vert3.x = j*16+15;
			noteonQuad.vert1.y = noteonQuad.vert3.y = 215;
			GPU_SortQuad(ot, 5, &noteonQuad);
		}

		// Process buffer
    	GPU_VSync();
    	GPU_DrawOt(ot);
    	GPU_DrawSync();
		//GPU_FlipDisplay();
		// update MOD
		PollMOD(&themod);
	}

    INT_ResetHandler();


    temp2 = 27;
    temp = temp2++;
    printf("%u\t",temp);
    printf("Hello there Jamie!\n");


	exit(0);
}

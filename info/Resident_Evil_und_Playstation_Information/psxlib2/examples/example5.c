// SPU lib example 5
// Upload and Play VAB bank samples

#include <syscall.h>
#include <psxtypes.h>
#include <libspu.h>

// declare the VAB as an external object for the linker
extern char jum1[];


int main(void)
{
	int i;
	PsxUInt32 j, k;
	PsxUInt8 numvags;

	// initialise the SPU
	SPU_Init();
	// turn it on
	SPU_Enable();
	// upload the VAB
	numvags = SPU_LoadVAB(&jum1, 0x2000);
	// set voice 0 volume
	SPU_VoiceVol(0, 0x3000, 0x3000);
	// set voice 0 pitch
	SPU_VoicePitch(0, 0x400);

	//SPU_VoiceOn(1);
	for(i=0; i < numvags; i++) {
		// get new sample
		SPU_VoiceVABSample(0, i);
		// play it!
		SPU_VoiceOn(0);
		for(j=0; j<100; j++) {
			for(k=0; k<100000; k++) ;
		}
		SPU_VoiceOff(0);
	}

//  	for(i=0; i<500000; i++) ;
//  	printf("%08X\n", SPU_VoiceActiveMask());
	
	exit(0);
}




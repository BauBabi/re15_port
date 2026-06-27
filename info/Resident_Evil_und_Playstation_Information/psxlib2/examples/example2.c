// SPU lib example 2
// Upload sample and reverb data, play sample with reverb

#include <psxtypes.h>
#include <syscall.h>
#include <libspu.h>

// declare the sample as an external object for the linker
extern char jum303[];

int main(void)
{

	// initialise the SPU
	SPU_Init();
	// turn it on
	SPU_Enable();

	// upload the sample
	SPU_UploadData(&jum303, 0x1010, 48600);
	// Tell voice 0 where to find the sample
	SPU_VoiceSetData(0, 0x1010);
	
	// Upload a reverb patch
	// (see libspu.h for #defines for reverb patches)
	SPU_LoadReverb(RVB_DELAY);
	// set reverb depth (left and right channels)
	SPU_ReverbDepth(0x6000, 0x7000);

	// set voice 0 ADSR
	// NB: check libspu.h and docs for expl. - tricky!
	SPU_VoiceADSR(0, 40, 8, 10, 0, 10);

	// set voice 0 volume
	SPU_VoiceVol(0, 0x3000, 0x3000);
	// set voice 0 sample playback speed

	SPU_VoicePitch(0, 0x1000);
	// turn voice 0 reverb on
	SPU_VoiceReverb(0);

	// play it!
	SPU_VoiceOn(0);



while(1)
  {
  }
	
return 0;
}




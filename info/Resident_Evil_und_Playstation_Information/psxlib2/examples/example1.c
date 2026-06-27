// SPU lib example 1
// Upload and Play sample

#include <syscall.h>
#include <psxtypes.h>
#include <libspu.h>

// declare the sample as an external object for the linker
extern char moon1[];

int main(void)
{

	// initialise the SPU
	SPU_Init();
	// turn it on
	SPU_Enable();
	// upload sample
	SPU_UploadData(&moon1, 65536, 207504);
	// Tell voice 0 where to find the sample
	SPU_VoiceSetData(0, 65536);
	// set voice 0 ADSR
	// NB: check libspu.h and docs for expl. - tricky!
	SPU_VoiceADSR(0, 40, 8, 10, 0, 10);
	// set voice 0 volume
	SPU_VoiceVol(0, 0x3000, 0x3000);
	// set voice 0 sample playback speed
	SPU_VoicePitch(0, 0x1000);
	// play it!
	SPU_VoiceOn(0);


while(1)
  {
  }
	
return 0;
}




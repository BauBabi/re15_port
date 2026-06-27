// SPU lib example 6
// Noise stuff

#include <syscall.h>
#include <psxtypes.h>
#include <libspu.h>

// waves
extern char sqr440[];
extern char sine440[];


int main(void)
{

	// initialise the SPU
	SPU_Init();
	// turn it on
	SPU_Enable();
	// set noise rate
	SPU_SetNoiseRate(0x3F);

	// upload the samples
	SPU_UploadData(&sqr440, 0x2000, 720);
	SPU_UploadData(&sine440, 0x3000, 720);

	// Tell voice 0 and 1 where to find the samples
	SPU_VoiceSetData(0, 0x3000);
	SPU_VoiceSetData(1, 0x2000);

	// set voice 0 pitch
	SPU_VoicePitch(0, 0x200);
	// set voice 1 pitch
	SPU_VoicePitch(1, 0x3000);
	// set voice 0 & 1 ADSR
	SPU_VoiceADSR(0, 40, 8, 10, 0, 10);
	SPU_VoiceADSR(1, 32, 8, 10, 0, 10);

	// set voice 1 to Noise
	SPU_Voice2Noise(1);
	// set voice 0 volume
	SPU_VoiceVol(0, 0x3000, 0x3000);
	// set voice 1 volume
	SPU_VoiceVol(1, 0x3000, 0x3000);

	// play it!
	SPU_VoiceOnMask(3);			// switch voice 0 and 1 on

while(1)
  {
  }
	
return 0;
}




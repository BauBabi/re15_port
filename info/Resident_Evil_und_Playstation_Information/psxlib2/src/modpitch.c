// generate MOD note-number to pitch tables

#include <stdio.h>
#include <string.h>

int main(void) {
	int nn;
	float fpitch;
	unsigned short pitch;

	for(nn=0; nn<1024; nn++) {
		fpitch = 7159091.0 / (nn*2);
		fpitch *= 4096.0;
		fpitch /= 44100.0;
		if(fpitch > 32767) fpitch = 32767;
		pitch = (unsigned short)fpitch;
		printf("0x%04X,\t\t// %d\n", pitch, nn);
	}
	return 0;

}

int sub04(void) {

	Evt_next();
	nop();
	if(Ck(4, 95, 0)) {
		Sce_espr3d_on(0, 0x16, 0x00, 0, 0, 0x8009, 25031, -14600, 9901, 0, 2, 0, 0);
	} else {
		Sce_espr3d_on(0, 0x16, 0x10, 0, 0, 0x8009, 25031, -14600, 9901, 0, 2, 0, 0);
		nop();
		nop();
		return 0;
}
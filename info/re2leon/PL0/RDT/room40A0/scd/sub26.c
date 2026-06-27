int sub26(void) {

	Aot_reset(4, 0, 0, 0, 0, 0x00);
	Message_on(0, 256, 0x00FF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Se_on(2, 16, 1, 0, 0, 29466, 4341, 25000);
		Evt_next();
		nop();
		Se_on(2, 17, 1, 0, 0, 29466, 4341, 25000);
		goto Sub22();
		Aot_reset(0, 5, 49, 65280, 6171, 0x00);
		Aot_reset(1, 5, 49, 65280, 6172, 0x00);
	} else {
		Se_on(4, 5, 0, 0, 0, 0, 0, 0);
		Aot_reset(4, 5, 49, 65280, 6170, 0x00);
		nop();
		nop();
		return 0;
}
int sub02(void) {

	Aot_reset(5, 0, 0, 0, 0, 0x00);
	Message_on(0, 0, 0x00FF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Message_on(0, 256, 0x00FF);
		Se_on(2, 10, 1, 0, 0, -28729, -30470, -13651);
	} else {
		Se_on(4, 5, 0, 0, 0, 0, 0, 0);
		nop();
		nop();
		Aot_reset(5, 5, 49, 65280, 6146, 0x00);
		return 0;
}
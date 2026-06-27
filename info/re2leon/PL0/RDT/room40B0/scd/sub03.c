int sub03(void) {

	Message_on(0, 0, 0x00FF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Se_on(4, 6, 0, 0, 0, 0, 0, 0);
		Aot_on(1);
	} else {
		Se_on(4, 5, 0, 0, 0, 0, 0, 0);
		nop();
		nop();
		return 0;
}
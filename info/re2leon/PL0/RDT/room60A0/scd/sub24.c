int sub24(void) {

	Set(2, 7, 1);
	Set(2, 2, 1);
	Set(2, 0, 1);
	Message_on(0, 2048, 0x00EF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Se_on(4, 6, 0, 0, 0, 0, 0, 0);
		goto Sub25();
	} else {
		Se_on(4, 5, 0, 0, 0, 0, 0, 0);
		nop();
		nop();
		Set(2, 7, 0);
		Set(2, 2, 0);
		Set(2, 0, 0);
		return 0;
}
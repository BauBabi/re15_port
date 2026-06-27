int sub03(void) {

	Message_on(0, 1024, 0xFFFF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Aot_on(5);
	}
	return 0;
}
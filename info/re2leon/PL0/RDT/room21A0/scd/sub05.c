int sub05(void) {

	Message_on(0, 768, 0xFFFF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Aot_on(2);
	}
	return 0;
}
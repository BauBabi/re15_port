int sub08(void) {

	do {
		Evt_next();
		nop();
	} while(Ck(5, 36, 0));
	return 0;
}
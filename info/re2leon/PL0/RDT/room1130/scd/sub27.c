int sub27(void) {

	do {
		Evt_next();
		nop();
	} while(Ck(5, 32, 0));
	return 0;
}
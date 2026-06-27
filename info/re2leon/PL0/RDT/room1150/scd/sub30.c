int sub30(void) {

	do {
		Evt_next();
		nop();
	} while(Ck(5, 33, 0));
	return 0;
}
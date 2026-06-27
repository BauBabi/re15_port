int sub09(void) {

	do {
		Evt_next();
		nop();
	} while(Ck(5, 35, 0));
	return 0;
}
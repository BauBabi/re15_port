int sub18(void) {

	Se_on(2, 13, 1, 4, 8, 0, 0, 0);
	for(n = 0; n < 10240; n++) {
		Evt_next();
		nop();
	}
	goto(0x0000EAFF);
	return 0;
}
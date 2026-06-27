int sub07(void) {

	Evt_next();
	Work_set(3, 0);
	Plc_stop();
	nop();
	return 0;
}
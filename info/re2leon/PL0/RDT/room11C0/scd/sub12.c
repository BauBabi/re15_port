int sub12(void) {

	Work_set(4, 2);
	nop();
	Speed_set(4, 2);
	Add_speed();
	Evt_next();
	goto(0x0000FEFF);
	return 0;
}
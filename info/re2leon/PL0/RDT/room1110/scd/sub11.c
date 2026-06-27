int sub11(void) {

	Sleep(2560);
	Se_on(2, 11, 1, 4, 6, 0, 0, 0);
	Work_set(4, 6);
	nop();
	Speed_set(5, 3072);
	Add_speed();
	Evt_next();
	goto(0x0000FEFF);
	return 0;
}
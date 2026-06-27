int sub07(void) {

	Work_set(4, 0);
	nop();
	Se_on(2, 13, 0, 4, 0, 0, 0, 0);
	Pos_set(0, 131, -3880, 131);
	Dir_set(0, 0, 14, -16384);
	Speed_set(0, -25596);
	Speed_set(6, -2305);
	Speed_set(2, -25596);
	Speed_set(8, -2305);
	Speed_set(1, 11265);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Evt_exec(255, 0x1808);
	Speed_set(1, -14336);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(0, 5120);
	Speed_set(2, 5120);
	Speed_set(1, 7680);
	for(n = 0; n < 12800; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, 2560);
	Speed_set(2, 2560);
	Speed_set(1, 7680);
	for(n = 0; n < 12800; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
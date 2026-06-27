int DOOR1D10(void) {

	Work_set(5, 1);
	nop();
	Speed_set(0, 0);
	Speed_set(2, -3585);
	Speed_set(6, 512);
	for(n = 0; n < 12800; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
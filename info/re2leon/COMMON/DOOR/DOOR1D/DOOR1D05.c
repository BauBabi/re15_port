int DOOR1D05(void) {

	Work_set(5, 2);
	nop();
	Speed_set(3, 0);
	Speed_set(9, -1);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
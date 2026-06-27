int sub15(void) {

	Work_set(4, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(6, 2560);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
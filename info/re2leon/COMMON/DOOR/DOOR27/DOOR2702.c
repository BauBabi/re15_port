int DOOR2702(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(8, -257);
	Speed_set(2, -1793);
	for(n = 0; n < 5120; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Speed_set(8, 512);
	for(n = 0; n < 5120; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	return 0;
}
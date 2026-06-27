int DOOR2703(void) {

	Work_set(5, 1);
	nop();
	Speed_set(0, 0);
	Speed_set(8, 512);
	Speed_set(2, 2048);
	for(n = 0; n < 5120; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Speed_set(8, -257);
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
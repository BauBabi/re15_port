int DOOR2C05(void) {

	Work_set(5, 1);
	nop();
	Speed_set(0, 0);
	Speed_set(8, -513);
	Speed_set(2, -1793);
	for(n = 0; n < 5120; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Speed_set(8, 768);
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
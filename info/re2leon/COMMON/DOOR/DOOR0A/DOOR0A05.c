int DOOR0A05(void) {

	Work_set(5, 1);
	nop();
	Speed_set(3, 0);
	Speed_set(9, -1);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Evt_next();
		Add_aspeed();
		nop();
	}
	Speed_set(9, 1536);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
		Add_aspeed();
		nop();
	}
	return 0;
}
int DOOR2803(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, -2305);
	Speed_set(6, -257);
	for(n = 0; n < 2560; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 43520; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(6, 512);
	for(n = 0; n < 1280; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		nop();
	}
	return 0;
}
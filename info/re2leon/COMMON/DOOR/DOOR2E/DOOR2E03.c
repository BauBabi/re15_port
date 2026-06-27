int DOOR2E03(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(10, 256);
	Speed_set(4, 1792);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
	}
	for(n = 0; n < 1280; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Speed_set(10, -1);
	for(n = 0; n < 1280; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
	}
	return 0;
}
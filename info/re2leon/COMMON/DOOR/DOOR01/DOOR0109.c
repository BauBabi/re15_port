int DOOR0109(void) {

	Work_set(5, 3);
	nop();
	Speed_set(2, 0);
	Speed_set(0, 0);
	Speed_set(6, 2560);
	Speed_set(8, 256);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(8, 0);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
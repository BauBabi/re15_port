int DOOR3007(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(6, 512);
	Speed_set(7, -1);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(7, 512);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(7, -513);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Sce_fade_set(0, 2, 7, 0, 4);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(7, 512);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}
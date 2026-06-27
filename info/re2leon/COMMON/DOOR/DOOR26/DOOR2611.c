int DOOR2611(void) {

	Work_set(5, 0);
	nop();
	Speed_set(4, -1);
	Speed_set(10, -257);
	for(n = 0; n < 2048; n++) {
		for(n = 0; n < 2560; n++) {
			Add_speed();
			Evt_next();
		}
		Add_aspeed();
		nop();
	}
	Speed_set(10, 512);
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
		nop();
	}
	for(n = 0; n < 1024; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Evt_next();
	}
	return 0;
}
int DOOR1B05(void) {

	Work_set(5, 1);
	nop();
	Speed_set(4, -1281);
	Speed_set(10, 256);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_aspeed();
		nop();
	}
	Sleep(4608);
	Speed_set(0, 0);
	Speed_set(10, -1);
	Speed_set(4, 0);
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
	for(n = 0; n < 3584; n++) {
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
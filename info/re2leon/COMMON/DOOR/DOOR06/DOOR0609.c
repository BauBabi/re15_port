int DOOR0609(void) {

	Work_set(5, 0);
	nop();
	Speed_set(4, 1536);
	Speed_set(10, -1);
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
	Speed_set(10, 256);
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
	Speed_set(10, -257);
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
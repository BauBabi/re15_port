int DOOR1403(void) {

	Work_set(5, 0);
	nop();
	Speed_set(8, -1);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(8, -257);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 4096; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(8, 512);
	for(n = 0; n < 12800; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}
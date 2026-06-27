int sub12(void) {

	Work_set(4, 0);
	nop();
	Speed_set(4, 3072);
	Speed_set(10, -257);
	for(n = 0; n < 512; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_aspeed();
		Add_aspeed();
	}
	Add_speed();
	Evt_next();
	Add_speed();
	Evt_next();
	Add_aspeed();
	Add_aspeed();
	Sleep(2304);
	Speed_set(0, 0);
	Speed_set(10, 512);
	Speed_set(4, 0);
	for(n = 0; n < 512; n++) {
		Add_aspeed();
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Evt_next();
		nop();
	}
	for(n = 0; n < 1792; n++) {
		Add_aspeed();
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
	Speed_set(10, -769);
	for(n = 0; n < 512; n++) {
		Add_aspeed();
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
	Add_aspeed();
	Add_aspeed();
	Add_speed();
	Evt_next();
	Add_speed();
	Evt_next();
	for(n = 0; n < 512; n++) {
		Add_aspeed();
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		Evt_next();
		nop();
	}
	return 0;
}
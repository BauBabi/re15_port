int DOOR3504(void) {

	Work_set(5, 0);
	nop();
	Speed_set(11, -1);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	Sleep(5120);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(11, 256);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	return 0;
}
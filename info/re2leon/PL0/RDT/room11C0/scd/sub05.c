int sub05(void) {

	Work_set(4, 0);
	nop();
	Speed_set(4, -1);
	Speed_set(10, -1);
	for(n = 0; n < 4608; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(10, 256);
	for(n = 0; n < 8192; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(10, -1);
	for(n = 0; n < 3584; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(4, -1025);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(4, -257);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
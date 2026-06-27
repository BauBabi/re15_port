int DOOR3105(void) {

	Work_set(5, 2);
	nop();
	Speed_set(9, -1281);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(9, 1024);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}
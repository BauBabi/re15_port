int DOOR3508(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(6, 2560);
	Speed_set(1, -3585);
	Speed_set(7, -1);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, -16641);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, -22273);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, -8193);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
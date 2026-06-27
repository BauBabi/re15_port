int DOOR3305(void) {

	Work_set(5, 0);
	nop();
	Speed_set(5, 0);
	Speed_set(0, 0);
	Speed_set(6, 1024);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 16640; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
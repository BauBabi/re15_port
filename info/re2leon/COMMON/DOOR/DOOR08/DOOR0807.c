int DOOR0807(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 25600);
	Speed_set(6, -2305);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}
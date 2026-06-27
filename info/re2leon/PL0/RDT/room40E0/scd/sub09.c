int sub09(void) {

	Work_set(4, 0);
	nop();
	Speed_set(1, -4865);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, -3585);
	for(n = 0; n < 4096; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, -2305);
	for(n = 0; n < 7936; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, -1025);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, 0);
	return 0;
}
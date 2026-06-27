int DOOR1603(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 12800);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
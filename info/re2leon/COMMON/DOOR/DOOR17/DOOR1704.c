int DOOR1704(void) {

	Work_set(5, 4);
	nop();
	Speed_set(5, -4097);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
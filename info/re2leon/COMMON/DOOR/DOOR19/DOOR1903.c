int DOOR1903(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 25600);
	for(n = 0; n < 30720; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
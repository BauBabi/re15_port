int DOOR2502(void) {

	Work_set(5, 0);
	nop();
	Speed_set(7, -1025);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 8960; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
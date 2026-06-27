int DOOR2A06(void) {

	for(n = 0; n < 5120; n++) {
		Add_aspeed();
		Add_speed();
		Evt_next();
		Add_speed();
		Evt_next();
		nop();
	}
	return 0;
}
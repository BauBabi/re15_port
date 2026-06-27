int DOOR2E06(void) {

	Work_set(5, 2);
	nop();
	Speed_set(3, -3329);
	Speed_set(9, -2305);
	for(n = 0; n < 512; n++) {
		Add_speed();
		Evt_next();
		Add_aspeed();
		nop();
	}
	for(n = 0; n < 7424; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}
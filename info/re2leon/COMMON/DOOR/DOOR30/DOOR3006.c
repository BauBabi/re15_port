int DOOR3006(void) {

	Work_set(5, 1);
	nop();
	Speed_set(0, 0);
	Speed_set(6, 512);
	Speed_set(7, 256);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(7, -257);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(7, 768);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(7, -257);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}
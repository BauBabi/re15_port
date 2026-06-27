int DOOR1005(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 2560);
	for(n = 0; n < 2560; n++) {
		for(n = 0; n < 1792; n++) {
			Add_speed();
			Evt_next();
			Save(4, 1);
		}
		Save(4, 65280);
		for(n = 0; n < 768; n++) {
			Add_speed();
			Evt_next();
			Save(4, 1);
		}
	}
	return 0;
}
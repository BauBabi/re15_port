int DOOR0705(void) {

	Work_set(5, 1);
	nop();
	Speed_set(3, -2817);
	Speed_set(9, -1281);
	for(n = 0; n < 512; n++) {
		Add_speed();
		Evt_next();
		Add_aspeed();
		nop();
	}
	for(n = 0; n < 7168; n++) {
		Add_speed();
		Evt_next();
	}
	Work_set(5, 0);
	nop();
	Speed_set(4, 4608);
	Speed_set(9, 0);
	Add_speed();
	nop();
	return 0;
}
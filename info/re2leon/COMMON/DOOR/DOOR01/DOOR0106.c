int DOOR0106(void) {

	Work_set(5, 2);
	nop();
	if(Cmp(14 = 0)) {
		Speed_set(3, -513);
		Speed_set(9, -769);
	} else {
		Speed_set(3, -1025);
		Speed_set(9, -1793);
		nop();
		nop();
		for(n = 0; n < 512; n++) {
			Add_speed();
			Evt_next();
			Add_aspeed();
			nop();
		}
		for(n = 0; n < 6656; n++) {
			Add_speed();
			Evt_next();
		}
		return 0;
}
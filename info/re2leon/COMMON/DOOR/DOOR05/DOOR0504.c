int DOOR0504(void) {

	Work_set(5, 2);
	nop();
	if(Cmp(14 = 0)) {
		Speed_set(3, 12800);
	} else {
		Speed_set(3, -1025);
		nop();
		nop();
		for(n = 0; n < 7680; n++) {
			Add_speed();
			Evt_next();
		}
		return 0;
}
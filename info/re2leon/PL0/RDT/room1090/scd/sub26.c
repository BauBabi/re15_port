int sub26(void) {

	Work_set(4, 1);
	nop();
	Speed_set(1, 12800);
	Speed_set(2, -20225);
	Speed_set(3, -27905);
	for(n = 0; n < 2816; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, 0);
	Speed_set(2, 0);
	Speed_set(3, 5120);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, 0);
	Speed_set(7, 5120);
	Speed_set(3, 0);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	return 0;
}
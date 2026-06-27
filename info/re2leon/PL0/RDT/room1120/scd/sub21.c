int sub21(void) {

	Work_set(4, 8);
	nop();
	Member_set(12, -5910);
	Speed_set(5, 2);
	Speed_set(1, 20480);
	Speed_set(0, -25345);
	Speed_set(7, 4096);
	for(n = 0; n < 512; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(0, -2305);
	Speed_set(4, 16384);
	Speed_set(5, -32768);
	while(Member_cmp(12 < 0)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	Member_set(12, -2305);
	Speed_set(7, 5120);
	Speed_set(1, -20225);
	Speed_set(0, -25345);
	Speed_set(2, -25345);
	Speed_set(5, 2);
	Speed_set(3, 16384);
	while(Member_cmp(12 < 0)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	Member_set(12, -19456);
	Member_set(14, 0);
	Member_set(15, 10);
	Member_set(16, 12);
	Speed_set(0, 0);
	Speed_set(2, 0);
	Speed_set(5, 0);
	Speed_set(1, -12545);
	Speed_set(5, 1);
	Add_speed();
	Evt_next();
	Speed_set(0, 0);
	Speed_set(5, 0);
	Speed_set(1, -4865);
	Speed_set(5, 255);
	Add_speed();
	Evt_next();
	Speed_set(0, 0);
	Speed_set(5, 0);
	Speed_set(1, 5120);
	Speed_set(5, 1);
	Add_speed();
	Evt_next();
	Speed_set(0, 0);
	Speed_set(5, 0);
	Speed_set(1, 12800);
	Speed_set(5, 255);
	Add_speed();
	nop();
	Member_set(14, 0);
	return 0;
}
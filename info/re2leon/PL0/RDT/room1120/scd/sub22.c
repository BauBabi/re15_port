int sub22(void) {

	Work_set(4, 11);
	nop();
	Member_set(12, -5910);
	Speed_set(5, 254);
	Speed_set(1, -19456);
	Speed_set(0, 25600);
	Speed_set(7, 6400);
	for(n = 0; n < 512; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(0, 2560);
	Speed_set(4, 16384);
	Speed_set(3, -32768);
	while(Member_cmp(12 < 0)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	Member_set(12, -2305);
	Speed_set(7, 5120);
	Speed_set(1, -25345);
	Speed_set(0, 5120);
	Speed_set(2, 5120);
	Speed_set(5, -32768);
	Speed_set(3, 2);
	while(Member_cmp(12 < 0)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	Member_set(12, -19456);
	Member_set(14, 0);
	Member_set(16, 12);
	Speed_set(0, 0);
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
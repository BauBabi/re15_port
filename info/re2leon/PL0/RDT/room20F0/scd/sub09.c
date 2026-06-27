int sub09(void) {

	Work_set(4, 4);
	nop();
	Speed_set(1, 15360);
	Speed_set(7, 7680);
	Se_on(2, 10, 1, 4, 4, 0, 0, 0);
	Member_set(12, -16145);
	Evt_next();
	nop();
	for(n = 0; n < 768; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 1024; n++) {
		Member_copy(16, 12);
		nop();
		Calc(16 + 37120)
		Member_set2(12, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 + 38400)
		Member_set2(12, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 + 39680)
		Member_set2(12, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 12);
		nop();
		Calc(16 + 38400)
		Member_set2(12, 16);
		nop();
		Evt_next();
		nop();
	}
	Member_copy(16, 12);
	nop();
	Calc(16 + 39680)
	Member_set2(12, 16);
	nop();
	Evt_next();
	nop();
	Member_copy(16, 12);
	nop();
	Calc(16 - 1280)
	Member_set2(12, 16);
	nop();
	return 0;
}
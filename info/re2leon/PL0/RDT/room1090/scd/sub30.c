int sub30(void) {

	Work_set(4, 5);
	nop();
	Speed_set(1, 12800);
	Speed_set(2, 20480);
	Speed_set(3, 25856);
	for(n = 0; n < 2816; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(1, 0);
	Speed_set(2, 0);
	Speed_set(3, 2560);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(3, -2305);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(3, 2560);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(3, -2305);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(3, 2560);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(3, -2305);
	for(n = 0; n < 2560; n++) {
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
	// unknown opcode (8A) found at 0x00000094
	Se_on(2, 14, 1, 4, 5, 0, 0, 0);
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	return 0;
}
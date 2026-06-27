int DOOR2D05(void) {

	Work_set(5, 2);
	nop();
	for(n = 0; n < 1024; n++) {
		Member_set(12, 7180);
		Member_set(11, -5910);
		// unknown opcode (8C) found at 0x0000001A
		for(n = 0; n < 15360; n++) {
			Member_copy(16, 12);
			nop();
			Calc(16 - 32000)
			Member_set2(12, 16);
			nop();
			Member_copy(16, 11);
			nop();
			Calc(16 + 51200)
			Member_set2(11, 16);
			nop();
			Evt_next();
			nop();
		}
		Sleep(5632);
	}
	Member_set(12, 7180);
	Member_set(11, -5910);
	Speed_set(0, -14336);
	Speed_set(1, -31745);
	Speed_set(6, -1793);
	Speed_set(7, 1280);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}
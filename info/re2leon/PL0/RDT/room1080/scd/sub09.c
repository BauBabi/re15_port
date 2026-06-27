int sub09(void) {

	Work_set(4, 0);
	nop();
	Pos_set(0, 28846, -1800, 941);
	Sleep(5120);
	Sleep(7680);
	// unknown opcode (8B) found at 0x0000001A
	// unknown opcode (8B) found at 0x00000020
	// unknown opcode (8B) found at 0x00000026
	// unknown opcode (8B) found at 0x0000002C
	// unknown opcode (8B) found at 0x00000032
	Speed_set(3, 0);
	Speed_set(9, -1);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(3, -2305);
	Speed_set(9, -1);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(9, -257);
	for(n = 0; n < 5888; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(9, 2560);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	for(n = 0; n < 1280; n++) {
		Sleep(4352);
		Speed_set(3, -7937);
		Speed_set(9, -257);
		for(n = 0; n < 2560; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
		Speed_set(9, 2048);
		for(n = 0; n < 1024; n++) {
			Add_speed();
			Add_aspeed();
			Evt_next();
			nop();
		}
	}
	while(Ck(5, 3, 0)) {
		Evt_next();
		nop();
	}
	Pos_set(0, 0, 0, 0);
	return 0;
}
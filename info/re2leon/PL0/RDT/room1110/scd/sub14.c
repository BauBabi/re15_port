int sub14(void) {

	// unknown opcode (8B) found at 0x00000006
	Se_on(2, 14, 1, 4, 0, 0, 0, 0);
	Work_set(4, 0);
	nop();
	Speed_set(3, 1024);
	for(n = 0; n < 16384; n++) {
		Add_speed();
		Evt_next();
	}
	Cut_chg(0x08);
	// unknown opcode (8A) found at 0x0000002C
	// unknown opcode (8B) found at 0x00000032
	// unknown opcode (8B) found at 0x00000038
	for(n = 0; n < 16384; n++) {
		Add_speed();
		Evt_next();
	}
	Cut_chg(0x00);
	goto Sub20();
	return 0;
}
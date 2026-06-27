int sub09(void) {

	// unknown opcode (8B) found at 0x00000006
	// unknown opcode (8B) found at 0x0000000C
	Member_copy(16, 32);
	nop();
	Calc(16 + 49665)
	Member_set2(32, 16);
	nop();
	Member_copy(16, 35);
	nop();
	Calc(16 + 49665)
	Member_set2(35, 16);
	nop();
	Speed_set(2, 3840);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		nop();
		Member_copy(16, 32);
		nop();
		Calc(16 - 2048)
		Member_set2(32, 16);
		nop();
		Member_copy(16, 35);
		nop();
		Calc(16 - 2048)
		Member_set2(35, 16);
		nop();
		Evt_next();
		Add_speed();
		Member_copy(16, 32);
		nop();
		Calc(16 - 1792)
		Member_set2(32, 16);
		nop();
		Member_copy(16, 35);
		nop();
		Calc(16 - 1792)
		Member_set2(35, 16);
		nop();
		Evt_next();
		nop();
	}
	Member_set(32, 16126);
	Member_set(35, -17406);
	return 0;
}
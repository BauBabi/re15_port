int sub09(void) {

	Sleep(2560);
	Work_set(4, 1);
	nop();
	Pos_set(0, 8929, 0, 212);
	Dir_set(0, 0, 0, 0);
	Evt_next();
	nop();
	Se_on(2, 11, 0, 0, 0, 8929, -1800, 212);
	// unknown opcode (8A) found at 0x0000002C
	// unknown opcode (8B) found at 0x00000032
	Save(4, 3326);
	Save(5, 39166);
	Save(9, 7680);
	for(n = 0; n < 2048; n++) {
		Member_copy(16, 11);
		nop();
		Calc2(0x10, 0x4);
		Member_set2(11, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc2(0x10, 0x5);
		Member_set2(12, 16);
		nop();
		Member_copy(16, 16);
		nop();
		Calc2(0x10, 0x9);
		Member_set2(16, 16);
		nop();
		Copy(16, 5);
		nop();
		Calc(16 + 23040)
		Copy(5, 16);
		nop();
		Evt_next();
		nop();
	}
	// unknown opcode (8A) found at 0x00000080
	// unknown opcode (8B) found at 0x00000086
	Save(9, 58111);
	Save(4, 40191);
	for(n = 0; n < 256; n++) {
		Member_copy(16, 16);
		nop();
		Calc2(0x10, 0x9);
		Member_set2(16, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc2(0x10, 0x5);
		Member_set2(12, 16);
		nop();
		Copy(16, 5);
		nop();
		Calc(16 + 23040)
		Copy(5, 16);
		nop();
		Evt_next();
		nop();
	}
	Save(5, 62206);
	Save(4, 2560);
	Save(9, 7680);
	Save(8, 58111);
	for(n = 0; n < 1792; n++) {
		Member_copy(16, 11);
		nop();
		Calc2(0x10, 0x4);
		Member_set2(11, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc2(0x10, 0x5);
		Member_set2(12, 16);
		nop();
		Copy(16, 5);
		nop();
		Calc(16 + 23040)
		Copy(5, 16);
		nop();
		Member_copy(16, 16);
		nop();
		Calc2(0x10, 0x9);
		Member_set2(16, 16);
		nop();
		Copy(16, 9);
		nop();
		Calc(16 + 7680)
		Copy(9, 16);
		nop();
		Member_copy(16, 15);
		nop();
		Calc2(0x10, 0x8);
		Member_set2(15, 16);
		nop();
		Evt_next();
		nop();
	}
	Evt_exec(255, 0x180A);
	// unknown opcode (8A) found at 0x0000012E
	// unknown opcode (8B) found at 0x00000134
	Save(5, 19711);
	Save(4, 3840);
	for(n = 0; n < 1280; n++) {
		Member_copy(16, 11);
		nop();
		Calc2(0x10, 0x4);
		Member_set2(11, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc2(0x10, 0x5);
		Member_set2(12, 16);
		nop();
		Copy(16, 5);
		nop();
		Calc(16 + 23040)
		Copy(5, 16);
		nop();
		Member_copy(16, 15);
		nop();
		Calc2(0x10, 0x8);
		Member_set2(15, 16);
		nop();
		Evt_next();
		nop();
	}
	// unknown opcode (8A) found at 0x0000017E
	// unknown opcode (8C) found at 0x00000186
	Save(5, 42751);
	Save(4, 3840);
	for(n = 0; n < 768; n++) {
		Member_copy(16, 11);
		nop();
		Calc2(0x10, 0x4);
		Member_set2(11, 16);
		nop();
		Member_copy(16, 12);
		nop();
		Calc2(0x10, 0x5);
		Member_set2(12, 16);
		nop();
		Copy(16, 5);
		nop();
		Calc(16 + 23040)
		Copy(5, 16);
		nop();
		Evt_next();
		nop();
	}
	return 0;
}
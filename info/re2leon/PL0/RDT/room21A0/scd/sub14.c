int sub14(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Cut_auto(0);
	Set(2, 2, 1);
	if(Ck(4, 114, 1)) {
		if(Ck(6, 79, 0)) {
			Work_set(3, 0);
			nop();
			Member_set(2, 256);
			Member_set(3, 512);
			Member_set(4, 0);
			Member_set(5, 0);
			Pos_set(0, -11830, 0, 21746);
			Member_copy(16, 1);
			nop();
			Calc(16 || 2048)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 || 128)
			Member_set2(7, 16);
			nop();
		}
		if(Ck(6, 80, 0)) {
			Work_set(3, 1);
			nop();
			Member_set(2, 256);
			Member_set(3, 512);
			Member_set(4, 0);
			Member_set(5, 0);
			Pos_set(0, 28631, 0, -17443);
			Member_copy(16, 1);
			nop();
			Calc(16 || 2048)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 || 128)
			Member_set2(7, 16);
			nop();
		}
	}
	Work_set(4, 4);
	nop();
	Super_set(0, 64, 139, 21760, -18432, 8704, 0, 8, -16384);
	Sca_id_set(21, 0x00DC);
	Sca_id_set(16, 0x00DC);
	Work_set(1, 0);
	nop();
	Pos_set(0, 16358, 0, -21776);
	Dir_set(0, 0, 28685, 0);
	Plc_motion(0, 15, 0);
	Plc_neck(2, 0, 0, 0, 0, 8224);
	Sleep(5120);
	Se_on(2, 13, 0, 0, 0, 16358, 0, -21776);
	// unknown opcode (8A) found at 0x000000F0
	// unknown opcode (8A) found at 0x000000F6
	// unknown opcode (8A) found at 0x000000FC
	// unknown opcode (8A) found at 0x00000102
	Sleep(11264);
	// unknown opcode (8A) found at 0x0000010C
	// unknown opcode (8B) found at 0x00000112
	// unknown opcode (8B) found at 0x00000118
	// unknown opcode (8C) found at 0x00000120
	// unknown opcode (8B) found at 0x00000126
	Evt_exec(255, 0x180F);
	Sleep(2816);
	Evt_exec(255, 0x1810);
	Sleep(14080);
	Aot_reset(2, 1, 49, 55511, 0, 0xC5BC);
	Aot_reset(0, 5, 49, 65280, 6149, 0x00);
	Work_set(1, 0);
	Plc_ret();
	Cut_auto(1);
	Set(1, 27, 0);
	Sce_Item_lost(75);
	Aot_on(12);
	Evt_next();
	Work_set(4, 4);
	Super_set(0, 0, 0, 0, 0, 0, 0, 0, 0);
	Set(2, 7, 0);
	Set(2, 2, 0);
	if(Ck(6, 79, 0)) {
		Work_set(3, 0);
		nop();
		Member_copy(16, 1);
		nop();
		Calc(16 && 63487)
		Member_set2(1, 16);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 && 65407)
		Member_set2(7, 16);
		nop();
	}
	if(Ck(6, 80, 0)) {
		Work_set(3, 1);
		nop();
		Member_copy(16, 1);
		nop();
		Calc(16 && 63487)
		Member_set2(1, 16);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 && 65407)
		Member_set2(7, 16);
		nop();
	}
	return 0;
}
int sub05(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Cut_chg(0x0F);
	if(Ck(1, 3, 1)) {
		Work_set(1, 0);
		Plc_ret();
		Pos_set(0, -609, 0, -15167);
		Member_set(15, 2053);
		Work_set(2, 0);
		nop();
		Pos_set(0, -10597, 0, 16578);
		Member_set(15, -10238);
	} else {
		Work_set(1, 0);
		Plc_ret();
		Pos_set(0, 30109, 0, 18114);
		Member_set(15, 6148);
		nop();
		nop();
		Sleep(5120);
		// unknown opcode (8A) found at 0x00000052
		// unknown opcode (8B) found at 0x00000058
		// unknown opcode (8B) found at 0x0000005E
		Se_on(2, 17, 1, 1, 0, 0, 0, 0);
		Set(1, 28, 1);
		Sleep(2048);
		Set(1, 28, 0);
		Sleep(23040);
		// unknown opcode (8A) found at 0x00000080
		// unknown opcode (8B) found at 0x00000086
		// unknown opcode (8B) found at 0x0000008C
		Se_on(2, 18, 1, 1, 0, 0, 0, 0);
		Set(1, 28, 1);
		Sleep(2048);
		Set(1, 28, 0);
		Sleep(5120);
		Set(2, 7, 0);
		Set(1, 27, 0);
		return 0;
}
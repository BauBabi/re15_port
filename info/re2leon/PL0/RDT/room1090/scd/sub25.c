int sub25(void) {

	// unknown opcode (8B) found at 0x00000006
	// unknown opcode (8C) found at 0x0000000E
	// unknown opcode (8B) found at 0x00000014
	// unknown opcode (8C) found at 0x0000001C
	Evt_exec(255, 0x1820);
	Sleep(1280);
	Evt_exec(255, 0x181A);
	Evt_exec(255, 0x181B);
	Evt_exec(255, 0x181C);
	Evt_exec(255, 0x181D);
	Evt_exec(255, 0x181E);
	Evt_exec(255, 0x181F);
	Cut_chg(0x0F);
	Set(1, 29, 1);
	Work_set(1, 0);
	nop();
	Pos_set(0, 7153, 0, 1016);
	Member_set(15, -1272);
	Plc_motion(10, 15, 0);
	Sleep(8960);
	Member_set(12, 0);
	Set(1, 29, 0);
	Sleep(20480);
	Work_set(3, 0);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65407)
	Member_set2(7, 16);
	nop();
	Cut_chg(0x0E);
	Cut_auto(1);
	Sca_id_set(21, 0x80FC);
	Work_set(1, 0);
	Plc_ret();
	Set(2, 7, 0);
	Set(1, 27, 0);
	Set(1, 25, 0);
	return 0;
}
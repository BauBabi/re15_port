int sub30(void) {

	Set(2, 7, 1);
	Set(4, 153, 1);
	Aot_reset(10, 0, 0, 0, 0, 0x00);
	Se_on(2, 1, 1, 0, 0, 3734, 4110, 29366);
	// unknown opcode (8A) found at 0x00000024
	// unknown opcode (8B) found at 0x0000002A
	Sleep(2560);
	Se_on(2, 1, 1, 0, 0, -16402, 4110, 29366);
	// unknown opcode (8A) found at 0x00000040
	// unknown opcode (8B) found at 0x00000046
	Set(1, 27, 1);
	goto Sub34();
	Cut_chg(0x05);
	Sce_bgm_control(0, 5, 0, 0, 65);
	Sleep(2560);
	Evt_exec(255, 0x1826);
	Sleep(11776);
	Evt_exec(255, 0x1823);
	Sleep(5120);
	Evt_exec(255, 0x1825);
	Sleep(10240);
	Evt_exec(255, 0x1824);
	Sleep(28160);
	Work_set(3, 0);
	nop();
	goto Sub31();
	Pos_set(0, 6336, 0, 1246);
	Work_set(3, 1);
	nop();
	goto Sub31();
	Pos_set(0, 6336, 0, 25288);
	Work_set(3, 2);
	nop();
	goto Sub31();
	Pos_set(0, 6336, 0, -4425);
	Work_set(3, 3);
	nop();
	goto Sub31();
	Pos_set(0, 6336, 0, -32550);
	Cut_old();
	Cut_chg(0x0F);
	nop();
	Evt_exec(255, 0x1828);
	Sleep(9216);
	Evt_exec(255, 0x182B);
	Sleep(17920);
	Evt_exec(255, 0x1829);
	Sleep(5120);
	Evt_exec(255, 0x182A);
	Sleep(28160);
	goto Sub53();
	Cut_old();
	Sce_bgm_control(0, 1, 0, 81, 65);
	Work_set(1, 0);
	Plc_ret();
	Set(2, 7, 0);
	Set(1, 27, 0);
	nop();
	return 0;
}
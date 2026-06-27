int sub32(void) {

	Set(2, 7, 1);
	Set(4, 153, 1);
	Aot_reset(10, 0, 0, 0, 0, 0x00);
	Se_on(2, 1, 1, 0, 0, 3734, 4110, 29366);
	// unknown opcode (8A) found at 0x00000024
	// unknown opcode (8B) found at 0x0000002A
	Sleep(2560);
	Set(1, 27, 1);
	goto Sub34();
	Cut_chg(0x05);
	Sce_bgm_control(0, 5, 0, 0, 65);
	Evt_exec(9, 0x181A);
	Work_set(4, 13);
	nop();
	Pos_set(0, 24761, 5371, 27335);
	Member_set(15, 0);
	Work_set(4, 14);
	nop();
	Pos_set(0, 24761, 5371, -19236);
	Member_set(15, 0);
	Work_set(4, 15);
	nop();
	Pos_set(0, 24761, 5371, 25330);
	Member_set(15, 0);
	Sleep(15360);
	Evt_kill(9);
	Evt_exec(255, 0x182E);
	Sleep(4096);
	Evt_exec(255, 0x1830);
	Evt_exec(255, 0x1823);
	Sleep(11776);
	Evt_exec(255, 0x1825);
	Sleep(10240);
	Evt_exec(255, 0x1826);
	Sleep(15360);
	Evt_exec(255, 0x1824);
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
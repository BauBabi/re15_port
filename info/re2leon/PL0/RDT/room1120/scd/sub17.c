int sub17(void) {

	Set(4, 53, 1);
	Set(2, 7, 1);
	Sca_id_set(22, 0x0000);
	Aot_reset(9, 0, 0, 0, 0, 0x00);
	Set(23, 1, 0);
	Set(23, 2, 1);
	Set(22, 1, 0);
	Evt_exec(255, 0x1813);
	Evt_exec(255, 0x1815);
	Evt_exec(255, 0x1817);
	Evt_exec(255, 0x1814);
	Evt_exec(255, 0x1816);
	Evt_exec(255, 0x1818);
	// unknown opcode (8A) found at 0x00000040
	// unknown opcode (8B) found at 0x00000046
	Se_on(2, 19, 1, 1, 0, 0, 0, 0);
	Work_set(1, 0);
	nop();
	Speed_set(1, 25600);
	Speed_set(7, 7680);
	Plc_motion(0, 18, 8);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Plc_motion(0, 16, 12);
	Cut_chg(0x0C);
	Cut_auto(1);
	Evt_exec(255, 0x1812);
	while(Member_cmp(12 <= 0)) {
		Evt_next();
		Add_speed();
		Add_aspeed();
		nop();
	}
	// unknown opcode (8A) found at 0x00000090
	// unknown opcode (8B) found at 0x00000096
	Se_on(2, 31, 1, 1, 0, 0, 0, 0);
	Member_set(12, 0);
	Member_set(17, 0);
	Plc_motion(0, 10, 8);
	Sleep(768);
	Plc_flg(1, 0, 0);
	Sleep(2048);
	Plc_cnt(36);
	Plc_flg(0, 144, 0);
	Sleep(1536);
	Plc_neck(4, 4, 0, 0, 0, 24672);
	Plc_cnt(11);
	Plc_flg(1, 16, 0);
	Sleep(2560);
	Plc_stop();
	nop();
	Sleep(7680);
	Plc_stop();
	nop();
	Sleep(15360);
	Plc_ret();
	Set(23, 3, 1);
	nop();
	Sca_id_set(21, 0x00DC);
	Set(2, 7, 0);
	return 0;
}
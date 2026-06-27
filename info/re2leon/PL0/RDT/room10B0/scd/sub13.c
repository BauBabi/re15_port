int sub13(void) {

	Cut_chg(0x06);
	Set(2, 7, 1);
	Work_set(3, 0);
	nop();
	Pos_set(0, 11436, -24355, 2739);
	Member_set(23, 256);
	Member_set(29, -27136);
	goto Sub14();
	Evt_next();
	nop();
	Sleep(1536);
	Se_on(2, 33, 0, 0, 0, -23894, -20245, -8516);
	// unknown opcode (8A) found at 0x00000034
	// unknown opcode (8B) found at 0x0000003A
	// unknown opcode (8C) found at 0x00000042
	// unknown opcode (8B) found at 0x00000048
	// unknown opcode (8C) found at 0x00000050
	// unknown opcode (8A) found at 0x00000056
	// unknown opcode (8A) found at 0x0000005C
	// unknown opcode (8A) found at 0x00000062
	// unknown opcode (8A) found at 0x00000068
	// unknown opcode (8A) found at 0x0000006E
	// unknown opcode (8A) found at 0x00000074
	// unknown opcode (8A) found at 0x0000007A
	// unknown opcode (8A) found at 0x00000080
	Se_on(3, 8, 0, 3, 0, 0, 0, 0);
	goto Sub15();
	Work_set(1, 0);
	nop();
	Plc_dest(0, 2336, 2219, 14269);
	Plc_rot(0, 1);
	Sleep(512);
	Work_set(1, 0);
	nop();
	Plc_neck(5, 3, 0, 0, 0, 24672);
	Plc_dest(0, 1312, 2219, 14269);
	Sleep(768);
	if(Ck(4, 43, 0)) {
		Cut_chg(0x02);
	} else {
		Cut_chg(0x0C);
		nop();
		nop();
		Set(15, 2, 0);
		Set(25, 2, 0);
		Work_set(3, 0);
		nop();
		Member_set(29, 25600);
		Pos_set(0, 1963, 16621, 14269);
		Set(1, 27, 1);
		Set(2, 7, 1);
		Sce_bgm_control(0, 1, 0, 0, 0);
		Sce_bgm_control(1, 1, 0, 0, 0);
		Work_set(1, 0);
		Plc_motion(0, 15, 0);
		Flr_set(1, 1);
		Sleep(1792);
		Work_set(1, 0);
		Plc_stop();
		Se_on(2, 10, 1, 0, 0, -23894, 0, -8516);
		Sleep(256);
		Work_set(1, 0);
		Plc_stop();
		Sleep(512);
		Sce_espr_on(0x00, 0x11, 5, 0, 0, 0x20, 8359, -1825, -3416, 4);
		Sleep(256);
		Work_set(1, 0);
		nop();
		Plc_flg(0, 16, 0);
		Sleep(512);
		Se_on(2, 10, 1, 0, 0, -23894, 0, -8516);
		Sleep(768);
		Sce_espr_on(0x00, 0x12, 5, 0, 0, 0x10, -10062, -7965, -21836, 4);
		Sleep(2560);
		Sce_espr_on(0x00, 0x11, 5, 0, 0, 0x30, 13484, -13090, 30405, 0);
		Work_set(1, 0);
		nop();
		Plc_dest(0, 2080, 32683, -7805);
		Plc_flg(0, 16, 0);
		Sleep(3840);
		Work_set(1, 0);
		nop();
		Plc_dest(0, 2336, 2219, 14269);
		Plc_rot(0, 1);
		Sleep(768);
		Work_set(1, 0);
		Plc_motion(0, 17, 0);
		nop();
		Plc_flg(0, 16, 0);
		Sleep(6400);
		Se_on(3, 8, 0, 3, 0, 0, 0, 0);
		Sleep(1792);
		Work_set(1, 0);
		nop();
		Plc_dest(0, 2080, 32683, -7805);
		Plc_flg(0, 16, 0);
		Sleep(10240);
		if(Ck(4, 43, 0)) {
			Cut_chg(0x04);
		} else {
			Cut_chg(0x0E);
			nop();
			nop();
			Cut_replace(2, 4);
			Cut_replace(12, 14);
			Cut_replace(1, 7);
			Set(1, 27, 0);
			Cut_auto(1);
			Work_set(3, 0);
			Member_set(23, 256);
			Work_set(1, 0);
			Plc_ret();
			Set(2, 7, 0);
			return 0;
}
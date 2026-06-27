int sub07(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Aot_reset(2, 0, 0, 0, 0, 0x00);
	Aot_reset(6, 0, 0, 0, 0, 0x00);
	Aot_reset(7, 0, 0, 0, 0, 0x00);
	Aot_reset(8, 0, 0, 0, 0, 0x00);
	Aot_reset(9, 0, 0, 0, 0, 0x00);
	Aot_reset(10, 0, 0, 0, 0, 0x00);
	Evt_next();
	Set(5, 2, 0);
	nop();
	Evt_exec(255, 0x1808);
	Set(5, 3, 0);
	Evt_exec(255, 0x1809);
	Sleep(2560);
	Sca_id_set(9, 0x0000);
	Sca_id_set(16, 0x0000);
	Sleep(2560);
	do {
		Evt_next();
		nop();
	} while(Ck(5, 2, 0));
	Cut_chg(0x05);
	Sleep(23040);
	// unknown opcode (8A) found at 0x0000007E
	// unknown opcode (8A) found at 0x00000084
	// unknown opcode (8C) found at 0x0000008C
	Se_on(2, 11, 1, 0, 0, -18762, 4341, -10579);
	Sce_bgm_control(1, 1, 1, 80, 64);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x20, 9366, -31535, -8015, 0);
	Sleep(1792);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x20, 9366, -31535, -8015, 0);
	Sleep(768);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x22, 9366, 19666, 28851, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x22, 9366, 5331, 181, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x24, 9366, 5331, -25420, 0);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x24, 9366, 16596, 8376, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x26, 22166, 16596, 8376, 0);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x26, 9366, -12075, -5960, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x28, 9366, -31535, 4786, 0);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x28, 9366, 5331, 181, 0);
	Sce_espr_on(0x00, 0x1A, 17, 0, 0, 0x28, 9366, 16596, -4425, 0);
	Sleep(512);
	Evt_exec(255, 0x180A);
	Cut_chg(0x03);
	Sce_espr_kill(26, 17, 0, 0);
	nop();
	Sce_espr_on(0x00, 0x23, 0, 0, 0, 0x2B, 29866, -17175, 13501, 0);
	Evt_next();
	nop();
	Sce_espr_on(0x00, 0x24, 0, 0, 0, 0x24, 29866, -16135, 27836, 0);
	Evt_next();
	nop();
	Sce_espr_on(0x00, 0x23, 0, 0, 0, 0x2B, 28586, 22761, 13501, 0);
	Evt_next();
	nop();
	Sce_espr_on(0x00, 0x24, 0, 0, 0, 0x24, 28586, -16135, 27836, 0);
	Sleep(7680);
	Sce_bgm_control(1, 5, 0, 0, 0);
	Sleep(15360);
	Cut_chg(0x08);
	Sleep(23040);
	Evt_exec(255, 0x180B);
	Cut_chg(0x07);
	Sce_espr_kill(35, 0, 0, 0);
	Sce_espr_kill(36, 0, 0, 0);
	Sleep(25600);
	Set(4, 0, 1);
	if(Ck(1, 1, 1)) {
		Aot_reset(1, 5, 49, 65280, 6158, 0x00);
	}
	Aot_reset(2, 4, 49, 256, 0, 0xFF);
	Cut_chg(0x0F);
	Set(1, 27, 0);
	Evt_exec(255, 0x180C);
	Sleep(5120);
	Cut_auto(1);
	Set(2, 7, 0);
	Set(5, 1, 1);
	Aot_on(4);
	Sleep(256);
	if(Ck(1, 1, 0)) {
		if(Ck(1, 0, 0)) {
			Sce_bgmtbl_set(0, 0x801, 0xFF, 0x00);
			Sce_bgmtbl_set(0, 0xA01, 0xFF00, 0x00);
		} else {
			Sce_bgmtbl_set(0, 0x801, 0xFF, 0x00);
			nop();
			nop();
		} else {
			if(Ck(1, 0, 0)) {
				Sce_bgmtbl_set(0, 0x801, 0xFF, 0x00);
				Sce_bgmtbl_set(0, 0x901, 0x1E07, 0x4000);
				Sce_bgmtbl_set(0, 0xA01, 0xFF00, 0x00);
			} else {
				Sce_bgmtbl_set(0, 0x801, 0xFF, 0x00);
				Sce_bgmtbl_set(0, 0x901, 0x1E07, 0x4080);
				nop();
				nop();
				nop();
				nop();
				Set(5, 3, 1);
				return 0;
}
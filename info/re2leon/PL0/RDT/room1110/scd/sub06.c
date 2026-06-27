int sub06(void) {

	Message_on(0, 1280, 0xFFFF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Se_on(2, 10, 1, 4, 6, 0, 0, 0);
		if(Ck(4, 76, 1)) {
			Ck(4, 55, 0)
			Set(4, 55, 1);
			Aot_reset(4, 0, 0, 0, 0, 0x00);
			Evt_exec(255, 0x180C);
			Sleep(2560);
			Se_on(2, 12, 1, 4, 6, 0, 0, 0);
			Sce_bgm_control(1, 0, 1, 110, 65);
			Sce_bgm_control(1, 1, 0, 128, 65);
			if(Ck(1, 0, 0)) {
				if(Ck(1, 1, 0)) {
					Sce_bgmtbl_set(0, 0x1101, 0xFF0B, 0x00);
					Sce_bgmtbl_set(0, 0x1902, 0xFF12, 0xC0);
				} else {
					Sce_bgmtbl_set(0, 0x1101, 0xFF0B, 0x00);
					Sce_bgmtbl_set(0, 0x1902, 0xFF12, 0xC0);
					nop();
					nop();
				} else {
					if(Ck(1, 1, 0)) {
						Sce_bgmtbl_set(0, 0x1101, 0xFF0B, 0x00);
					} else {
						Sce_bgmtbl_set(0, 0x1101, 0xFF0B, 0x00);
						Sce_bgmtbl_set(0, 0x1701, 0x1EFF, 0x4000);
						nop();
						nop();
						nop();
						nop();
						Sleep(7680);
						Cut_chg(0x06);
						Set(1, 27, 1);
						Set(22, 1, 1);
						Sleep(12800);
						Work_set(1, 0);
						Plc_motion(1, 6, 128);
						nop();
						Sleep(2560);
						if(Ck(1, 0, 0)) {
							Pos_set(0, 11689, -5910, -19312);
							Member_set(15, -4092);
						} else {
							Pos_set(0, 15530, -5910, -8048);
							Member_set(15, -4092);
							nop();
							nop();
							Cut_chg(0x0A);
							Sleep(7680);
							Evt_exec(255, 0x1808);
							Sleep(2560);
							Work_set(1, 0);
							nop();
							Plc_dest(0, 5408, -16377, -32768);
							Sleep(23040);
							Set(22, 1, 0);
							Set(5, 0, 1);
							Set(1, 27, 0);
						} else {
							Evt_exec(9, 0x180B);
							Sleep(23040);
							Evt_kill(9);
							Sleep(7680);
							Work_set(4, 6);
							nop();
							Member_set(16, 0);
							nop();
							nop();
						}
						return 0;
}
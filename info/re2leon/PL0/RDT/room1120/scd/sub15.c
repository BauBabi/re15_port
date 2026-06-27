int sub15(void) {

	Message_on(0, 0, 0xFFFF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Set(2, 7, 1);
		Se_on(2, 10, 1, 0, 0, 19090, -1800, 451);
		if(Ck(4, 23, 0)) {
			Set(4, 23, 1);
			Set(5, 12, 1);
			Sleep(1280);
			// unknown opcode (8B) found at 0x0000003A
			// unknown opcode (8B) found at 0x00000040
			Se_on(2, 12, 1, 4, 3, 0, 0, 0);
			Work_set(4, 3);
			nop();
			Speed_set(2, -7425);
			for(n = 0; n < 3840; n++) {
				Add_speed();
				Evt_next();
			}
			Cut_chg(0x04);
			for(n = 0; n < 3840; n++) {
				Add_speed();
				Evt_next();
			}
			Sleep(3584);
			goto Sub16();
			Sleep(3840);
			Set(5, 12, 0);
			Cut_old();
			nop();
		} else {
			Set(4, 23, 0);
			Cut_chg(0x04);
			Work_set(1, 0);
			nop();
			if(Member_cmp(13 < -21310)) {
				Member_set(13, -21310);
			}
			Sleep(3840);
			Sce_espr_kill(22, 0, 4, 0);
			Sce_espr_kill(22, 0, 4, 1);
			Sce_espr_kill(22, 0, 4, 2);
			nop();
			Sleep(3840);
			// unknown opcode (8B) found at 0x000000B6
			// unknown opcode (8B) found at 0x000000BC
			Se_on(2, 12, 1, 4, 3, 0, 0, 0);
			Work_set(4, 3);
			nop();
			Speed_set(2, 7680);
			for(n = 0; n < 3840; n++) {
				Add_speed();
				Evt_next();
			}
			if(Ck(4, 24, 1)) {
				Cut_chg(0x0C);
				Evt_exec(255, 0x180E);
				for(n = 0; n < 3840; n++) {
					Add_speed();
					Evt_next();
				}
				Sleep(3840);
				Cut_chg(0x0E);
				Cut_auto(1);
			} else {
				Cut_chg(0x0E);
				Cut_auto(1);
				for(n = 0; n < 3840; n++) {
					Add_speed();
					Evt_next();
				}
				nop();
				nop();
				nop();
				nop();
				Set(2, 7, 0);
			}
			return 0;
}
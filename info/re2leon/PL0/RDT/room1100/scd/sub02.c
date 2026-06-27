int sub02(void) {

	if(Ck(4, 148, 0)) {
		Message_on(0, 512, 0xFFFF);
		Evt_next();
		nop();
		if(Ck(11, 31, 0)) {
			Set(2, 7, 1);
			Set(2, 2, 1);
			Set(2, 3, 1);
			Set(1, 27, 1);
			Work_set(1, 0);
			nop();
			Member_copy(4, 11);
			nop();
			Member_copy(5, 13);
			nop();
			Member_copy(6, 15);
			nop();
			goto Sub03();
			Set(4, 148, 1);
			Aot_reset(2, 1, 49, 31945, 63736, 0x3819);
			Cut_chg(0x0E);
			Work_set(4, 0);
			nop();
			Pos_set(0, -26420, 26821, -22501);
			Work_set(4, 1);
			nop();
			Pos_set(0, -26420, 26821, -22501);
			Work_set(4, 2);
			nop();
			Pos_set(0, -26420, 26821, -22501);
			Work_set(1, 0);
			nop();
			Pos_set(0, -1329, -16160, 27428);
			Dir_set(0, 0, 4, 0);
			Se_on(2, 10, 1, 0, 0, 27843, -1800, -26857);
			Sleep(7680);
			Se_on(2, 11, 1, 0, 0, 27843, -1800, -26857);
			// unknown opcode (8A) found at 0x000000A4
			// unknown opcode (8B) found at 0x000000AA
			// unknown opcode (8B) found at 0x000000B0
			// unknown opcode (8C) found at 0x000000B8
			for(n = 0; n < 14080; n++) {
				Work_set(4, 0);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Work_set(4, 1);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Work_set(4, 2);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Evt_next();
				nop();
			}
			Work_set(4, 0);
			nop();
			for(n = 0; n < 1280; n++) {
				Member_copy(16, 15);
				nop();
				Calc(16 - 1280)
				Member_set2(15, 16);
				nop();
				Evt_next();
				nop();
				Member_copy(16, 15);
				nop();
				Calc(16 + 1280)
				Member_set2(15, 16);
				nop();
				Evt_next();
				nop();
			}
			for(n = 0; n < 1024; n++) {
				Work_set(4, 1);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Work_set(4, 2);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Evt_next();
				nop();
			}
			Se_on(2, 11, 1, 0, 0, 27843, -1800, -26857);
			// unknown opcode (8A) found at 0x00000164
			// unknown opcode (8B) found at 0x0000016A
			// unknown opcode (8B) found at 0x00000170
			// unknown opcode (8C) found at 0x00000178
			for(n = 0; n < 14080; n++) {
				Work_set(4, 1);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Work_set(4, 2);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Evt_next();
				nop();
			}
			Work_set(4, 2);
			nop();
			Member_set(12, 31959);
			Work_set(4, 1);
			nop();
			Member_set(12, 31959);
			for(n = 0; n < 1280; n++) {
				Member_copy(16, 15);
				nop();
				Calc(16 - 1280)
				Member_set2(15, 16);
				nop();
				Evt_next();
				nop();
				Member_copy(16, 15);
				nop();
				Calc(16 + 1280)
				Member_set2(15, 16);
				nop();
				Evt_next();
				nop();
			}
			for(n = 0; n < 1024; n++) {
				Work_set(4, 2);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Evt_next();
				nop();
			}
			Se_on(2, 11, 1, 0, 0, 27843, -1800, -26857);
			// unknown opcode (8A) found at 0x0000020C
			// unknown opcode (8B) found at 0x00000212
			// unknown opcode (8B) found at 0x00000218
			// unknown opcode (8C) found at 0x00000220
			for(n = 0; n < 14080; n++) {
				Work_set(4, 2);
				nop();
				Member_copy(16, 12);
				nop();
				Calc(16 + 10240)
				Member_set2(12, 16);
				nop();
				Evt_next();
				nop();
			}
			Work_set(4, 2);
			nop();
			Member_set(12, -1824);
			for(n = 0; n < 1280; n++) {
				Member_copy(16, 15);
				nop();
				Calc(16 - 1280)
				Member_set2(15, 16);
				nop();
				Evt_next();
				nop();
				Member_copy(16, 15);
				nop();
				Calc(16 + 1280)
				Member_set2(15, 16);
				nop();
				Evt_next();
				nop();
			}
			Work_set(4, 0);
			nop();
			Pos_set(0, -26420, -7965, -22501);
			Work_set(4, 1);
			nop();
			Pos_set(0, 26675, -7965, 22756);
			Work_set(4, 2);
			nop();
			Pos_set(0, 26675, -7965, 22756);
			Work_set(1, 0);
			nop();
			Member_set2(11, 4);
			nop();
			Member_set(12, -7965);
			Member_set2(13, 5);
			nop();
			Member_set2(15, 6);
			nop();
			Cut_chg(0x0D);
			Cut_auto(1);
			goto Sub04();
			Set(1, 27, 0);
			Set(2, 7, 0);
			Set(2, 2, 0);
			Set(2, 3, 0);
		}
	} else {
		Message_on(0, 768, 0xFFFF);
		Evt_next();
		nop();
		if(Ck(11, 31, 0)) {
			Aot_on(2);
		}
		nop();
		nop();
		return 0;
}
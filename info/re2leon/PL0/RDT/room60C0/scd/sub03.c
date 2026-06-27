int sub03(void) {

	if(Ck(4, 96, 0)) {
		Message_on(0, 0, 0xFFFF);
		Evt_next();
		nop();
		if(Ck(11, 31, 0)) {
			Se_on(2, 10, 1, 1, 0, 0, 0, 0);
			if(Ck(4, 106, 0)) {
				Message_on(0, 256, 0xFFFF);
				Evt_next();
				nop();
			} else {
				// unknown opcode (8C) found at 0x00000040
				// unknown opcode (8B) found at 0x00000046
				// unknown opcode (8A) found at 0x0000004C
				// unknown opcode (8C) found at 0x00000054
				Set(2, 7, 1);
				Set(1, 27, 1);
				Aot_reset(10, 0, 0, 0, 0, 0x00);
				Work_set(4, 0);
				nop();
				Pos_set(0, -15317, 0, -13645);
				Work_set(1, 0);
				nop();
				Pos_set(0, 0, 0, 0);
				Cut_replace(8, 3);
				Cut_replace(7, 5);
				Cut_chg(0x03);
				Sleep(5120);
				Sce_espr_kill2(1);
				Sce_espr3d_on2(1, 0x16, 0x10, 0, 0, 0x10, -456, 20216, 30381, 0, 254, 0, 0);
				Se_on(2, 11, 0, 4, 0, 0, 0, 0);
				Evt_exec(255, 0x1804);
				while(Ck(5, 0, 0)) {
					Evt_next();
					nop();
				}
				Cut_chg(0x05);
				Cut_auto(1);
				Set(16, 1, 0);
				Set(16, 2, 0);
				Work_set(1, 0);
				nop();
				Pos_set(0, 19765, 0, -6992);
				Set(2, 7, 0);
				Set(1, 27, 0);
				nop();
				nop();
			} else {
				Se_on(4, 5, 1, 1, 0, 0, 0, 0);
				nop();
				nop();
			} else {
				Message_on(0, 512, 0xFFFF);
				Evt_next();
				nop();
				nop();
				nop();
				return 0;
}
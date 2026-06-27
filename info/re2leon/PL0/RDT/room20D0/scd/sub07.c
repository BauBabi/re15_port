int sub07(void) {

	if(Ck(5, 10, 1)) {
		Message_on(0, 1280, 0xFFFF);
		Evt_next();
		nop();
		if(Ck(11, 31, 0)) {
			Aot_reset(5, 0, 0, 0, 0, 0x00);
			Set(2, 7, 1);
			Set(2, 2, 1);
			Se_on(2, 10, 0, 0, 0, 0, 0, 0);
			Sleep(768);
			Set(2, 7, 0);
			Set(2, 2, 0);
			if(Ck(5, 13, 0)) {
				Set(5, 13, 1);
				Se_on(2, 11, 0, 0, 0, 0, 0, 0);
				Sce_espr_on2(0x03, 0x1E, 0, 0, 1, 0x20, -7495, -32525, -15145, 0);
				if(Cmp(4 = 256)) {
					Save(4, 512);
				}
				if(Ck(5, 12, 1)) {
					Ck(5, 13, 1)
					Ck(5, 11, 1)
					goto Sub09();
				}
			} else {
				Set(5, 13, 0);
				Se_on(2, 12, 0, 0, 0, 0, 0, 0);
				Sce_espr_kill2(3);
				Save(4, 0);
				nop();
				nop();
			}
		} else {
			Message_on(0, 2048, 0xFFFF);
			nop();
			nop();
			return 0;
}
int sub06(void) {

	if(Ck(5, 10, 1)) {
		Message_on(0, 1024, 0xFFFF);
		Evt_next();
		nop();
		if(Ck(11, 31, 0)) {
			Aot_reset(4, 0, 0, 0, 0, 0x00);
			Set(2, 7, 1);
			Set(2, 2, 1);
			Se_on(2, 10, 0, 0, 0, 0, 0, 0);
			Sleep(768);
			Set(2, 7, 0);
			Set(2, 2, 0);
			if(Ck(5, 12, 0)) {
				Set(5, 12, 1);
				Se_on(2, 11, 0, 0, 0, 0, 0, 0);
				Sce_espr_on2(0x02, 0x1E, 0, 0, 3, 0x20, -24405, -32525, -15145, 0);
				if(Cmp(4 = 0)) {
					Save(4, 256);
				}
				if(Ck(5, 12, 1)) {
					Ck(5, 13, 1)
					Ck(5, 11, 1)
					goto Sub09();
				}
			} else {
				Set(5, 12, 0);
				Se_on(2, 12, 0, 0, 0, 0, 0, 0);
				Sce_espr_kill2(2);
				Save(4, 0);
				nop();
				nop();
			}
		} else {
			Message_on(0, 1792, 0xFFFF);
			nop();
			nop();
			return 0;
}
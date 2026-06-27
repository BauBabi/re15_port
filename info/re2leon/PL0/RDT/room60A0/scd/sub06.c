int sub06(void) {

	Sleep(768);
	Save(8, 0);
	while(Ck(5, 0, 0)) {
		if(Ck(7, 19, 1)) {
			Set(5, 0, 1);
		}
		if(Ck(4, 131, 1)) {
			Set(5, 0, 1);
		}
		if(Ck(29, 13, 1)) {
			Set(5, 0, 1);
		}
		Copy(16, 8);
		nop();
		Calc(16 - 256)
		Copy(8, 16);
		nop();
		if(Cmp(8 <= 0)) {
			if(Cmp(29 >= 120)) {
				if(Cmp(29 ? 256)) {
					Se_on(2, 10, 1, 3, 0, 0, 0, 0);
					Save(8, 7680);
				} else {
					Se_on(2, 10, 1, 3, 0, 0, 0, 0);
					Save(8, 5120);
					nop();
					nop();
				} else {
					if(Cmp(29 ? 256)) {
						Se_on(2, 10, 1, 3, 0, 0, 0, 0);
						Save(8, 17920);
					} else {
						Se_on(2, 10, 1, 3, 0, 0, 0, 0);
						Save(8, 12800);
						nop();
						nop();
						nop();
						nop();
					}
					Evt_next();
					nop();
				}
				if(Ck(1, 1, 0)) {
					Set(29, 13, 1);
				}
				Set(4, 131, 1);
				Aot_reset(4, 0, 0, 0, 0, 0x00);
				Aot_reset(5, 0, 0, 0, 0, 0x00);
				Aot_reset(6, 0, 0, 0, 0, 0x00);
				Aot_reset(7, 0, 0, 0, 0, 0x00);
				Aot_reset(30, 0, 0, 0, 0, 0x00);
				Aot_reset(1, 1, 49, 55757, 0, 0x99E7);
				return 0;
}
int sub11(void) {

	Work_set(3, 0);
	Sce_bgm_control(0, 1, 0, 120, 64);
	nop();
	Member_copy(16, 23);
	nop();
	Calc(16 || 256)
	Member_set2(23, 16);
	nop();
	Evt_next();
	nop();
	Pos_set(0, -13039, -1800, -15432);
	Set(5, 2, 1);
	Se_on(2, 10, 1, 3, 0, 0, 0, 0);
	while(Ck(5, 20, 0)) {
		if(Ck(7, 0, 1)) {
			Set(5, 20, 1);
			Sce_bgm_control(0, 5, 0, 0, 0);
		} else {
			if(Ck(5, 21, 0)) {
				if(Cmp(26 = 2304)) {
					Set(5, 21, 1);
					Set(2, 7, 1);
					Evt_exec(255, 0x180D);
					Sce_bgm_control(0, 5, 0, 0, 0);
				}
			} else {
				if(Ck(1, 27, 0)) {
					Set(5, 20, 1);
					Set(2, 7, 0);
					Aot_reset(6, 4, 49, 1024, 0, 0xFF);
				}
				nop();
				nop();
				nop();
				nop();
				Evt_next();
				nop();
			}
			if(Ck(7, 0, 1)) {
				Set(2, 7, 1);
			}
			do {
				Evt_next();
				nop();
			} while(Ck(5, 30, 0));
			Set(2, 7, 0);
			Aot_reset(3, 0, 0, 0, 0, 0x00);
			return 0;
}
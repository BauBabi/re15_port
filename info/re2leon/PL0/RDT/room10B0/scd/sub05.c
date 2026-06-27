int sub05(void) {

	if(Ck(4, 18, 0)) {
		if(Cmp(0 = 3584)) {
			if(Cmp(2 = 13056)) {
				Set(4, 18, 1);
				Evt_exec(255, 0x180A);
			}
		}
	}
	if(Ck(4, 19, 0)) {
		if(Cmp(0 = 3840)) {
			if(Cmp(2 = 13056)) {
				Set(4, 19, 1);
				Evt_exec(255, 0x180B);
			}
		}
	}
	if(Ck(1, 1, 1)) {
		if(Keep_Item_ck(53);) {
			if(Ck(4, 171, 0)) {
				if(Cmp(1 = 3328)) {
					Set(4, 171, 1);
					Evt_exec(255, 0x180D);
					goto Sub02();
				}
			}
		}
		if(Ck(6, 20, 1)) {
			if(Ck(5, 3, 0)) {
				if(Ck(5, 2, 0)) {
					Set(5, 2, 1);
					Sce_bgm_control(1, 2, 0, 0, 0);
				}
			}
		}
		if(Ck(5, 3, 1)) {
			goto Sub07();
		}
	}
	return 0;
}
int sub02(void) {

	Set(2, 2, 1);
	Set(2, 7, 1);
	Cut_chg(0x0D);
	Work_set(1, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 4)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	Set(5, 5, 0);
	Set(5, 6, 0);
	Set(5, 7, 0);
	Set(5, 8, 0);
	Message_on(0, 3328, 0x007F);
	Evt_next();
	nop();
	Evt_exec(255, 0x1803);
	if(Ck(11, 29, 0)) {
		if(Ck(11, 30, 0)) {
			if(Ck(11, 31, 1)) {
				Set(5, 5, 1);
			}
		}
	}
	Message_on(0, 3584, 0x007F);
	Evt_next();
	nop();
	Evt_exec(255, 0x1803);
	if(Ck(11, 29, 0)) {
		if(Ck(11, 30, 0)) {
			if(Ck(11, 31, 1)) {
				Set(5, 6, 1);
			}
		}
	}
	Message_on(0, 3840, 0x007F);
	Evt_next();
	nop();
	Evt_exec(255, 0x1803);
	if(Ck(11, 29, 0)) {
		if(Ck(11, 30, 1)) {
			if(Ck(11, 31, 0)) {
				Set(5, 7, 1);
			}
		}
	}
	Message_on(0, 4096, 0x007F);
	Evt_next();
	nop();
	Evt_exec(255, 0x1803);
	if(Ck(11, 29, 1)) {
		if(Ck(11, 30, 0)) {
			if(Ck(11, 31, 1)) {
				Set(5, 8, 1);
			}
		}
	}
	if(Ck(5, 5, 1)) {
		if(Ck(5, 6, 1)) {
			if(Ck(5, 7, 1)) {
				if(Ck(5, 8, 1)) {
					Set(5, 9, 1);
				}
			}
		}
	}
	if(Ck(5, 9, 1)) {
		Set(4, 37, 1);
		Se_on(2, 11, 1, 1, 0, 0, 0, 0);
		Aot_reset(9, 5, 49, 65280, 6148, 0x00);
		Aot_on(9);
	} else {
		Cut_chg(0x0A);
		Work_set(1, 0);
		nop();
		Member_copy(16, 0);
		nop();
		Calc(16 && 65531)
		Member_set2(0, 16);
		nop();
		Member_copy(16, 1);
		nop();
		Calc(16 && 63487)
		Member_set2(1, 16);
		nop();
		Set(2, 7, 0);
		Set(2, 2, 0);
		Cut_auto(1);
		nop();
		nop();
		return 0;
}
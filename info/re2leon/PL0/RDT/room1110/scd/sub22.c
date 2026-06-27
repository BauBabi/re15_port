int sub22(void) {

	Set(2, 7, 1);
	if(Ck(4, 55, 0)) {
		Message_on(0, 1536, 0xFFFF);
		Evt_next();
		nop();
	} else {
		if(Ck(8, 55, 0)) {
			Aot_on(2);
			Evt_next();
			nop();
			if(Ck(8, 55, 1)) {
				Ck(1, 0, 0)
				Message_on(0, 1792, 0xFFFF);
				Evt_next();
				nop();
				if(Ck(11, 31, 0)) {
					Aot_on(1);
				}
			}
		} else {
			if(Ck(1, 0, 0)) {
				Message_on(0, 1792, 0xFFFF);
				Evt_next();
				nop();
				if(Ck(11, 31, 0)) {
					Aot_on(1);
				}
			}
			nop();
			nop();
			nop();
			nop();
			Set(2, 7, 0);
			return 0;
}
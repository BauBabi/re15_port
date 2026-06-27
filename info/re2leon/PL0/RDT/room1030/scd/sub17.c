int sub17(void) {

	Evt_next();
	Work_set(1, 0);
	if(Member_cmp(9 = 512)) {
		if(Ck(6, 56, 1)) {
			if(Ck(6, 60, 1)) {
				if(Ck(5, 0, 0)) {
					Set(5, 0, 1);
					Evt_exec(255, 0x180D);
				}
			}
		}
	}
	if(Cmp(26 = 2560)) {
		Sca_id_set(8, 0x005E);
	} else {
		if(Cmp(26 = 2304)) {
			Sca_id_set(8, 0x005E);
		} else {
			Sca_id_set(8, 0x00FE);
			nop();
			nop();
			nop();
			nop();
			goto(0x00009EFF);
			return 0;
}
int sub18(void) {

	Evt_next();
	nop();
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
			goto(0x0000D2FF);
			return 0;
}
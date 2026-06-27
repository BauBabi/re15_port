int sub14(void) {

	Set(2, 7, 1);
	Copy(16, 7);
	nop();
	Calc(16 + 256)
	Copy(7, 16);
	nop();
	if(Ck(8, 209, 0)) {
		if(Cmp(7 >= 12800)) {
			Aot_on(29);
			Save(7, 13056);
			Evt_next();
			nop();
		} else {
			Message_on(0, 2560, 0xFFFF);
			Evt_next();
			nop();
			nop();
			nop();
		} else {
			Message_on(0, 2560, 0xFFFF);
			Evt_next();
			nop();
			nop();
			nop();
			Set(2, 7, 0);
			return 0;
}
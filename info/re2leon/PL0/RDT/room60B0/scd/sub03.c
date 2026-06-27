int sub03(void) {

	Aot_reset(5, 0, 0, 0, 0, 0x00);
	Set(2, 7, 1);
	Message_on(0, 0, 0x00FF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Se_on(2, 10, 1, 0, 0, -28729, -30470, -13651);
		Evt_next();
		nop();
		Aot_reset(6, 0, 0, 0, 0, 0x00);
		Cut_chg(0x08);
		Work_set(1, 0);
		nop();
		Member_copy(16, 1);
		nop();
		Calc(16 || 2048)
		Member_set2(1, 16);
		nop();
		Cut_be_set(2, 2, 0);
		Cut_be_set(2, 3, 1);
		Cut_be_set(6, 1, 0);
		Cut_be_set(6, 2, 1);
		Cut_be_set(8, 1, 0);
		Cut_be_set(8, 3, 1);
		Set(1, 27, 1);
		Evt_exec(255, 0x1805);
		while(Ck(5, 0, 0)) {
			Evt_next();
			nop();
		}
		Set(1, 27, 0);
		Work_set(1, 0);
		nop();
		Member_copy(16, 1);
		nop();
		Calc(16 ^ 2048)
		Member_set2(1, 16);
		nop();
		Cut_chg(0x07);
		Set(21, 1, 0);
		Set(21, 3, 0);
		Cut_auto(1);
	} else {
		Se_on(4, 5, 0, 0, 0, 0, 0, 0);
		Aot_reset(5, 5, 49, 65280, 6147, 0x00);
		nop();
		nop();
		Set(2, 7, 0);
		return 0;
}
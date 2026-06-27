int sub09(void) {

	Set(2, 7, 1);
	Set(2, 2, 1);
	Set(2, 3, 1);
	Cut_chg(0x08);
	if(Ck(4, 19, 0)) {
		Message_on(0, 0, 0xFFFF);
	} else {
		Message_on(0, 1536, 0xFFFF);
		nop();
		nop();
		Evt_next();
		nop();
		if(Ck(4, 43, 0)) {
			Cut_chg(0x03);
		} else {
			Cut_chg(0x0D);
			nop();
			nop();
			Cut_auto(1);
			Set(2, 3, 0);
			Set(2, 2, 0);
			Set(2, 7, 0);
			return 0;
}
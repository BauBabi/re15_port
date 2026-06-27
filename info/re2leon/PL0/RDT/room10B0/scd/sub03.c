int sub03(void) {

	Set(2, 7, 1);
	Set(2, 2, 1);
	Set(2, 3, 1);
	if(Ck(4, 18, 1)) {
		Ck(4, 19, 1)
		if(Ck(8, 95, 0)) {
			Cut_chg(0x0B);
			Sleep(7680);
			Aot_on(16);
			Evt_next();
			Cut_chg(0x0D);
			Cut_auto(1);
			nop();
		} else {
			Cut_chg(0x0B);
			Message_on(0, 1792, 0xFFFF);
			Evt_next();
			Cut_chg(0x0D);
			Cut_auto(1);
			nop();
			nop();
			nop();
		} else {
			Cut_chg(0x0A);
			Message_on(0, 256, 0xFFFF);
			Evt_next();
			Cut_chg(0x03);
			Cut_auto(1);
			nop();
			nop();
			nop();
			Set(2, 3, 0);
			Set(2, 2, 0);
			Set(2, 7, 0);
			return 0;
}
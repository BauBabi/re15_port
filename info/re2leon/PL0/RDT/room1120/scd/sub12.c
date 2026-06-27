int sub12(void) {

	if(Ck(4, 24, 0)) {
		Cut_chg(0x0D);
		Set(2, 7, 1);
		Evt_next();
		nop();
		Message_on(0, 512, 0xFFFF);
		Evt_next();
		Cut_old();
		Set(2, 7, 0);
	} else {
		if(Ck(8, 21, 0)) {
			Cut_chg(0x0D);
			Set(2, 7, 1);
			Sleep(3840);
			Aot_on(11);
			Cut_old();
			Evt_next();
			Set(2, 7, 0);
		}
		nop();
		nop();
		return 0;
}
int sub02(void) {

	Message_on(0, 256, 0xFFFF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Set(4, 93, 1);
		Aot_reset(2, 4, 49, 512, 0, 0xFFFF);
		if(Ck(1, 0, 0)) {
			Aot_reset(0, 1, 49, 57521, 0, 0xC6A8);
		} else {
			Aot_reset(5, 1, 49, 131, 131, 0x83);
			nop();
			nop();
			Aot_reset(3, 0, 49, 0, 0, 0x00);
			Se_on(2, 15, 0, 0, 0, 2261, 24826, 23231);
			Sleep(2560);
			Se_on(2, 14, 0, 0, 0, 2261, 24826, 23231);
			Cut_chg(0x09);
			Cut_auto(1);
			goto Sub05();
			Evt_next();
			nop();
		}
		return 0;
}
int sub27(void) {

	Aot_reset(0, 1, 49, 18959, 0, 0xBEAE);
	if(Ck(1, 1, 0)) {
		if(Ck(7, 0, 1)) {
			Set(29, 3, 1);
		}
	}
	if(Ck(5, 30, 1)) {
		Set(4, 156, 1);
		Set(4, 155, 0);
	}
	Aot_on(0);
	if(Ck(1, 0, 0)) {
		Sce_bgmtbl_set(0, 0xA04, 0xFF00, 0x00);
	} else {
		Sce_bgmtbl_set(0, 0xA04, 0xFF00, 0x00);
		nop();
		nop();
		return 0;
}
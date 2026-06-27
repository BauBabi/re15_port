int sub19(void) {

	Set(2, 7, 1);
	Set(2, 2, 1);
	if(Ck(1, 0, 0)) {
		Aot_reset(13, 2, 49, 4096, 25600, 0x3300);
	} else {
		Aot_reset(13, 2, 49, 7424, 9216, 0x3300);
		nop();
		nop();
		Se_on(2, 13, 1, 1, 0, 0, 0, 0);
		Aot_on(13);
		Evt_next();
		nop();
		if(Ck(34, 51, 0)) {
			Aot_reset(13, 5, 49, 65280, 6163, 0x00);
		}
		Set(2, 7, 0);
		Set(2, 2, 0);
		return 0;
}
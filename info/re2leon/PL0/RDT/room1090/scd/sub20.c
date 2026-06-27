int sub20(void) {

	if(Ck(31, 16, 1)) {
		if(Ck(6, 105, 1)) {
			goto Sub21();
		}
		Set(6, 105, 1);
		Aot_reset(2, 1, 49, 53657, 57571, 0xDA9D);
		Aot_on(2);
	} else {
		Aot_reset(2, 1, 49, 53657, 57571, 0xDA9D);
		Aot_on(2);
		Aot_reset(2, 5, 49, 65280, 6164, 0x00);
		nop();
		nop();
		return 0;
}
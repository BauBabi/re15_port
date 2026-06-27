int sub23(void) {

	for(n = 0; n < 1536; n++) {
		Work_set(1, 0);
		nop();
		Member_copy(16, 15);
		nop();
		Calc(16 + 2560)
		Member_set2(15, 16);
		nop();
		Evt_next();
		nop();
	}
	return 0;
}
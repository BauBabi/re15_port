int sub16(void) {

	Work_set(1, 0);
	nop();
	for(n = 0; n < 5120; n++) {
		Member_copy(16, 11);
		nop();
		Calc(16 - 5888)
		Member_set2(11, 16);
		nop();
		Member_copy(16, 13);
		nop();
		Calc(16 - 8448)
		Member_set2(13, 16);
		nop();
		Evt_next();
		nop();
	}
	return 0;
}
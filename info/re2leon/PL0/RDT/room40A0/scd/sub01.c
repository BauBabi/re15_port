int sub01(void) {

	if(Ck(1, 29, 1)) {
		Ck(5, 16, 0)
		Set(5, 16, 1);
		Evt_exec(255, 0x180F);
	}
	return 0;
}
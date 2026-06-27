int sub01(void) {

	if(Cmp(26 = 1536)) {
		if(Ck(5, 2, 0)) {
			Set(5, 2, 1);
			Evt_exec(255, 0x1806);
		}
		if(Ck(5, 3, 0)) {
			goto Sub07();
		}
	} else {
		Set(5, 2, 0);
		if(Ck(5, 3, 1)) {
			goto Sub08();
		}
		nop();
		nop();
		return 0;
}
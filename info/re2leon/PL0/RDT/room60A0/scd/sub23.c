int sub23(void) {

	if(Poison_ck();) {
		nop();
		Message_on(0, 1792, 0x00EF);
		Evt_next();
		Poison_clr();
	} else {
		Message_on(0, 1536, 0x00EF);
		Evt_next();
		nop();
		nop();
		nop();
		return 0;
}
int sub22(void) {

	if(Ck(5, 4, 0)) {
		Set(5, 4, 1);
		Message_on(0, 0, 0xFFFF);
	} else {
		if(Ck(8, 13, 0)) {
			Aot_on(3);
		} else {
			Message_on(0, 0, 0xFFFF);
			nop();
			nop();
			nop();
			nop();
			return 0;
}
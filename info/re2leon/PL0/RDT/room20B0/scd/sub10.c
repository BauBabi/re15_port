int sub10(void) {

	Message_on(0, 0, 0x00FF);
	Evt_next();
	nop();
	if(Ck(11, 31, 0)) {
		Se_on(4, 6, 1, 4, 8, 0, 0, 0);
		Set(8, 83, 1);
		Set(33, 2, 1);
		Set(33, 3, 1);
		Set(33, 4, 1);
		Set(35, 2, 1);
		Set(35, 3, 1);
		Set(35, 4, 1);
		Work_set(4, 8);
		nop();
		Pos_set(0, 8270, 8270, 8270);
		Evt_next();
		nop();
		Message_on(0, 4352, 0x00FF);
	} else {
		Se_on(4, 5, 1, 4, 8, 0, 0, 0);
		nop();
		nop();
		return 0;
}
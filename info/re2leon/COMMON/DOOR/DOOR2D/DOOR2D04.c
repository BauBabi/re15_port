int DOOR2D04(void) {

	for(n = 0; n < 1024; n++) {
		Work_set(5, 2);
		nop();
		Member_set(12, -12050);
		Member_set(11, -14311);
		for(n = 0; n < 15360; n++) {
			Work_set(5, 2);
			nop();
			Member_copy(16, 12);
			nop();
			Calc(16 + 32000)
			Member_set2(12, 16);
			nop();
			Member_copy(16, 11);
			nop();
			Calc(16 - 51200)
			Member_set2(11, 16);
			nop();
			Evt_next();
			nop();
		}
		Sleep(5120);
	}
	return 0;
}
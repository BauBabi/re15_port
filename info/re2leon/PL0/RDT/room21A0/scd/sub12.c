int sub12(void) {

	if(Ck(5, 2, 1)) {
		Se_on(2, 14, 0, 0, 0, 19681, -1800, -23581);
		Set(5, 2, 0);
	} else {
		Se_on(2, 10, 0, 0, 0, 19681, -1800, -23581);
		nop();
		nop();
		Work_set(4, 2);
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 - 7680)
		Member_set2(11, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 + 4096)
		Member_set2(11, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 + 2048)
		Member_set2(11, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 + 1024)
		Member_set2(11, 16);
		nop();
		Evt_next();
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 + 512)
		Member_set2(11, 16);
		nop();
		Evt_next();
		Set(5, 3, 0);
		nop();
		return 0;
}
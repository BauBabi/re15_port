int sub13(void) {

	if(Ck(4, 61, 1)) {
		Se_on(2, 10, 0, 0, 0, 8929, -1800, 212);
	}
	Work_set(4, 1);
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
	Set(5, 4, 0);
	nop();
	return 0;
}
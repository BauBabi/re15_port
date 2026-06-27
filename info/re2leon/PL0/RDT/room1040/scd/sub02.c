int sub02(void) {

	do {
		Evt_next();
		nop();
	} while(Cmp(26 != 256));
	Work_set(3, 3);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65407)
	Member_set2(7, 16);
	nop();
	do {
		Evt_next();
		nop();
	} while(Cmp(26 != 768));
	Work_set(3, 4);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65407)
	Member_set2(7, 16);
	nop();
	do {
		Evt_next();
		nop();
	} while(Cmp(26 != 1024));
	Work_set(3, 2);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65407)
	Member_set2(7, 16);
	nop();
	return 0;
}
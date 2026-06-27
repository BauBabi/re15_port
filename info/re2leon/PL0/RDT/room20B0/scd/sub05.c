int sub05(void) {

	do {
		Evt_next();
		nop();
		if(Cmp(26 = 1280)) {
			Set(5, 32, 1);
		}
		if(Cmp(26 = 1536)) {
			Set(5, 32, 1);
		}
	} while(Ck(5, 32, 0));
	Work_set(3, 2);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 && 64511)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65471)
	Member_set2(7, 16);
	nop();
	Set(4, 28, 1);
	return 0;
}
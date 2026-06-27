int sub09(void) {

	Work_set(3, 1);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Pos_set(0, -21597, 0, -1024);
	Member_set(15, -18161);
	Save(7, 0);
	Save(8, 0);
	goto Sub10();
	Sleep(2560);
	Member_set(2, 2048);
	Member_set(3, 256);
	Member_set(4, 0);
	Member_set(5, 0);
	Member_copy(16, 7);
	nop();
	Calc(16 && 65407)
	Member_set2(7, 16);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 || 64)
	Member_set2(7, 16);
	nop();
	Member_set(20, -31554);
	Member_set(21, -17921);
	for(n = 0; n < 8192; n++) {
		Copy(16, 7);
		nop();
		Calc(16 + 1028)
		Copy(7, 16);
		nop();
		Copy(16, 8);
		nop();
		Calc(16 + 1024)
		Copy(8, 16);
		nop();
		goto Sub10();
		Evt_next();
		nop();
	}
	return 0;
}
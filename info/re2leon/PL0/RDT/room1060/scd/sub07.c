int sub07(void) {

	Work_set(3, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Pos_set(0, -18523, 0, -2562);
	Member_set(15, 12288);
	Save(4, 0);
	Save(5, 0);
	goto Sub08();
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
		Copy(16, 4);
		nop();
		Calc(16 + 1028)
		Copy(4, 16);
		nop();
		Copy(16, 5);
		nop();
		Calc(16 + 1024)
		Copy(5, 16);
		nop();
		goto Sub08();
		Evt_next();
		nop();
	}
	return 0;
}
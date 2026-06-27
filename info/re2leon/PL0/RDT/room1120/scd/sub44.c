int sub44(void) {

	Member_copy(16, 0);
	nop();
	Calc(16 || 4)
	Calc(16 || 512)
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Member_set(15, 8);
	Member_set(17, 2560);
	Member_set(23, 256);
	Save(7, 0);
	goto Sub45();
	Evt_next();
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 && 63487)
	Member_set2(1, 16);
	nop();
	for(n = 0; n < 4096; n++) {
		Evt_next();
		nop();
		Copy(16, 7);
		nop();
		Calc(16 + 2056)
		Copy(7, 16);
		nop();
		goto Sub45();
	}
	Se_on(2, 2, 1, 0, 0, 6336, -1800, 25288);
	Sleep(21504);
	Se_on(2, 3, 1, 0, 0, 6336, 0, 25288);
	Member_copy(16, 0);
	nop();
	Calc(16 && 65531)
	Member_set2(0, 16);
	nop();
	return 0;
}
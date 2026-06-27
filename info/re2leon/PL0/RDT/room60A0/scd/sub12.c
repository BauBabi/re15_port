int sub12(void) {

	Sce_espr_on(0x00, 0x00, 21, 0, 0, 0x0A, -29505, 22761, 18626, 0);
	Sleep(6400);
	Sce_espr_on(0x00, 0x00, 21, 0, 0, 0x0A, -29505, 22761, 18626, 0);
	Sleep(768);
	Sce_espr_on(0x00, 0x00, 21, 0, 0, 0x0A, -29505, 22761, 18626, 0);
	Sleep(10240);
	Work_set(4, 0);
	nop();
	Pos_set(0, -20557, -21780, -20544);
	Work_set(4, 1);
	nop();
	Pos_set(0, -19022, -1300, 32192);
	Work_set(4, 2);
	nop();
	Pos_set(0, -31822, 3821, 6592);
	Work_set(4, 3);
	nop();
	Pos_set(0, -19022, 3821, 32192);
	Se_on(2, 14, 1, 4, 0, 0, 0, 0);
	for(n = 0; n < 1024; n++) {
		Save(5, 0);
		for(n = 0; n < 1024; n++) {
			Work_copy(5, 2, 0);
			Work_set(4, 0);
			nop();
			Member_copy(16, 11);
			nop();
			Calc(16 + 25860)
			Member_set2(11, 16);
			nop();
			Member_copy(16, 13);
			nop();
			Calc(16 + 11265)
			Member_set2(13, 16);
			nop();
			Copy(16, 5);
			nop();
			Calc(16 + 256)
			Copy(5, 16);
			nop();
		}
		Evt_next();
		nop();
	}
	while(Ck(5, 3, 0)) {
		Evt_next();
		nop();
	}
	Work_set(4, 0);
	nop();
	Pos_set(0, 8270, 8270, -12281);
	Work_set(4, 1);
	nop();
	Pos_set(0, 8270, 8270, -12281);
	Work_set(4, 2);
	nop();
	Pos_set(0, 8270, 8270, -12281);
	Work_set(4, 3);
	nop();
	Pos_set(0, 8270, 8270, -12281);
	return 0;
}
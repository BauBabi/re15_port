int sub04(void) {

	Aot_reset(9, 0, 0, 0, 0, 0x00);
	Set(2, 2, 1);
	Set(2, 7, 1);
	Sce_fade_set(2, 2, 7, 0, 16);
	Sleep(1792);
	Sce_fade_set(2, 2, 7, 0, 240);
	Sleep(256);
	Cut_chg(0x0E);
	Se_on(2, 11, 1, 1, 0, 0, 0, 0);
	Work_set(4, 5);
	nop();
	Pos_set(0, -1582, -25345, 11475);
	Member_copy(16, 0);
	nop();
	Calc(16 || 4096)
	Member_set2(0, 16);
	nop();
	if(Ck(8, 83, 0)) {
		Work_set(4, 8);
		nop();
		Pos_set(0, -1582, -12545, -1326);
		Member_copy(16, 0);
		nop();
		Calc(16 || 4096)
		Member_set2(0, 16);
		nop();
	}
	Work_set(1, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 4)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 || 2048)
	Member_set2(1, 16);
	nop();
	Sleep(7680);
	if(Ck(8, 78, 0)) {
		Aot_on(10);
		Evt_next();
		Set(2, 7, 1);
		Set(2, 2, 1);
		nop();
		Sleep(7424);
	}
	if(Ck(8, 83, 0)) {
		Aot_on(11);
		Evt_next();
		Set(2, 7, 1);
		Set(2, 2, 1);
		nop();
		Sleep(7424);
	}
	Sce_fade_set(2, 2, 7, 0, 16);
	Sleep(1792);
	Sce_fade_set(2, 2, 7, 0, 240);
	Sleep(256);
	Cut_chg(0x0A);
	Se_on(2, 12, 1, 1, 0, 0, 0, 0);
	Work_set(4, 5);
	nop();
	Pos_set(0, 8270, 8270, 8270);
	Work_set(4, 8);
	nop();
	Pos_set(0, 8270, 8270, 8270);
	Work_set(1, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 && 65531)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 && 63487)
	Member_set2(1, 16);
	nop();
	if(Ck(8, 78, 1)) {
		Ck(8, 83, 1)
		Aot_reset(9, 0, 0, 0, 0, 0x00);
	} else {
		Aot_reset(9, 5, 49, 65280, 6148, 0x00);
		nop();
		nop();
		Set(2, 7, 0);
		Set(2, 2, 0);
		Cut_auto(1);
		return 0;
}
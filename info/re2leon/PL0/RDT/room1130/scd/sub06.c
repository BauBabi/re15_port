int sub06(void) {

	Set(4, 129, 1);
	Aot_reset(16, 0, 49, 46267, 0, 0xA899);
	if(Ck(6, 82, 0)) {
		Work_set(3, 1);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 && 65471)
		Member_set2(7, 16);
		nop();
		Sleep(3840);
	}
	if(Ck(6, 83, 0)) {
		Work_set(3, 2);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 && 65471)
		Member_set2(7, 16);
		nop();
		Sleep(7680);
	}
	if(Ck(6, 113, 0)) {
		Work_set(3, 3);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 && 65471)
		Member_set2(7, 16);
		nop();
		Sleep(30720);
	}
	if(Ck(6, 114, 0)) {
		Work_set(3, 4);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 && 65471)
		Member_set2(7, 16);
		nop();
	}
	return 0;
}
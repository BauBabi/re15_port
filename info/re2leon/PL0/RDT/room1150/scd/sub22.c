int sub22(void) {

	Cut_chg(0x06);
	Work_set(1, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Member_set2(0, 16);
	nop();
	Pos_set(0, 934, 0, 15284);
	Dir_set(0, 0, 15128, 0);
	Plc_motion(0, 15, 0);
	Work_set(4, 9);
	nop();
	Super_set(0, 64, 129, -24320, 9473, 23809, 4, 16629, 0);
	Work_set(3, 0);
	nop();
	Pos_set(0, 672, 0, 20659);
	Dir_set(0, 0, 31502, 0);
	Sleep(512);
	return 0;
}
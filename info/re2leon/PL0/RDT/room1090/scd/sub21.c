int sub21(void) {

	Set(2, 7, 1);
	Sce_fade_set(2, 2, 7, 0, 4);
	Sleep(5120);
	Cut_chg(0x0D);
	Work_set(3, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Pos_set(0, 31221, 0, 10230);
	Member_set(15, 4);
	Member_set(23, 256);
	Sce_fade_set(2, 2, 7, 0, 254);
	Sleep(30720);
	Member_set(23, 256);
	Sleep(30720);
	Se_on(2, 13, 0, 3, 0, 0, 0, 0);
	Sleep(20480);
	Set(2, 7, 0);
	return 0;
}
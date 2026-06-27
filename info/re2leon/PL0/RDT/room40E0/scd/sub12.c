int sub12(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	if(Ck(1, 3, 1)) {
		Work_set(2, 0);
		nop();
		Member_copy(16, 7);
		nop();
		Calc(16 + 128)
		Member_set2(7, 16);
		nop();
	}
	Cut_auto(0);
	Cut_chg(0x0B);
	Set(24, 5, 1);
	Set(24, 6, 1);
	goto Sub11();
	Sce_fade_set(1, 2, 7, 255, 241);
	Work_set(4, 0);
	nop();
	Pos_set(0, 131, 131, 131);
	Set(5, 0, 1);
	Cut_be_set(2, 2, 0);
	Cut_replace(1, 6);
	Cut_replace(2, 7);
	Cut_replace(3, 8);
	Cut_replace(4, 9);
	Cut_replace(5, 10);
	Work_set(1, 0);
	Pos_set(0, -17695, 0, -32035);
	Dir_set(0, 0, -13816, 0);
	if(Ck(1, 3, 1)) {
		Work_set(2, 0);
		nop();
		Pos_set(0, 12006, 0, 2781);
		Dir_set(0, 0, -13816, 0);
		Member_copy(16, 7);
		nop();
		Calc(16 - 128)
		Member_set2(7, 16);
		nop();
	}
	Cut_chg(0x0A);
	Set(24, 5, 0);
	Set(24, 6, 0);
	Cut_auto(1);
	Sleep(4096);
	Set(2, 7, 0);
	Set(1, 27, 0);
	return 0;
}
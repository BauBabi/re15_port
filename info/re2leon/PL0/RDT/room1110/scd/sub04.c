int sub04(void) {

	Set(4, 76, 1);
	Set(2, 7, 1);
	Work_set(1, 0);
	Plc_motion(1, 6, 0);
	nop();
	Sleep(6144);
	Cut_chg(0x07);
	goto Sub02();
	goto Sub03();
	Work_set(4, 7);
	nop();
	Member_copy(16, 12);
	nop();
	Calc(16 + 8270)
	Member_set2(12, 16);
	nop();
	Se_on(2, 15, 1, 4, 7, 0, 0, 0);
	Sce_Item_lost(74);
	Sleep(7680);
	goto Sub06();
	Cut_chg(0x06);
	Cut_auto(1);
	Work_set(1, 0);
	nop();
	if(Ck(5, 0, 0)) {
		Plc_motion(1, 6, 128);
		Sleep(6144);
	}
	Plc_ret();
	Set(5, 0, 0);
	Set(2, 7, 0);
	nop();
	return 0;
}
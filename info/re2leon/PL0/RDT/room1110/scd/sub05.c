int sub05(void) {

	Set(2, 7, 1);
	Work_set(1, 0);
	Plc_motion(1, 6, 0);
	nop();
	Sleep(6144);
	Cut_chg(0x07);
	goto Sub02();
	goto Sub03();
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
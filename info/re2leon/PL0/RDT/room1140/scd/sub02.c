int sub02(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Sleep(5120);
	Evt_exec(9, 0x1806);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 2337, 16546, 5791);
	goto Sub07();
	Plc_motion(2, 2, 4);
	Sleep(5120);
	Xa_on(0, 10496);
	Wsleep();
	Wsleeping();
	Cut_chg(0x03);
	Sleep(256);
	Sce_bgm_control(1, 1, 0, 0, 0);
	Sce_bgmtbl_set(0, 0x1401, 0xFF00, 0x00);
	if(Ck(1, 1, 1)) {
		Work_set(1, 0);
		nop();
		Pos_set(0, -1380, 0, 3781);
	}
	Evt_kill(9);
	Evt_exec(9, 0x1804);
	goto Sub03();
	if(Ck(1, 1, 0)) {
		Cut_chg(0x05);
	} else {
		Cut_chg(0x04);
		nop();
		nop();
		Evt_kill(9);
		goto Sub05();
		goto Sub10();
		return 0;
}
int sub02(void) {

	Set(2, 7, 1);
	Aot_reset(1, 0, 65, 0, 0, 0x00);
	if(Ck(1, 1, 0)) {
		Cut_chg(0x09);
		Sleep(7680);
		Aot_on(7);
		Evt_next();
		Set(2, 7, 1);
		Work_set(4, 3);
		Pos_set(0, -5713, -10502, 5054);
		Evt_next();
		nop();
		Sleep(10240);
		Aot_on(2);
		Cut_chg(0x85);
		Evt_next();
		Set(2, 7, 1);
		Evt_next();
		goto Sub03();
		Set(4, 15, 1);
		Cut_chg(0x02);
	} else {
		Cut_chg(0x09);
		Sleep(7680);
		Aot_on(7);
		Cut_chg(0x85);
		Evt_next();
		nop();
		nop();
		nop();
		Cut_auto(1);
		Set(2, 7, 0);
		return 0;
}
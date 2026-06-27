int sub02(void) {

	Aot_reset(5, 0, 0, 0, 0, 0x00);
	Set(4, 51, 1);
	Set(2, 7, 1);
	Set(1, 27, 1);
	Aot_reset(2, 5, 49, 65280, 6153, 0x00);
	Evt_exec(255, 0x1804);
	Evt_exec(255, 0x1803);
	Cut_chg(0x0A);
	while(Ck(5, 11, 0)) {
		Evt_next();
		nop();
	}
	Cut_chg(0x0B);
	Sleep(7680);
	Cut_chg(0x0A);
	Sleep(15360);
	Sleep(4608);
	Evt_exec(255, 0x181F);
	Evt_exec(255, 0x180B);
	Evt_exec(255, 0x180C);
	Sleep(10240);
	Aot_reset(4, 5, 49, 65280, 6170, 0x00);
	Set(1, 27, 0);
	return 0;
}